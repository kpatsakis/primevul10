int http_request_forward_body(struct session *s, struct buffer *req, int an_bit)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &s->txn.req;

	if (unlikely(msg->msg_state < HTTP_MSG_BODY))
		return 0;

	if ((req->flags & (BF_READ_ERROR|BF_READ_TIMEOUT|BF_WRITE_ERROR|BF_WRITE_TIMEOUT)) ||
	    ((req->flags & BF_SHUTW) && (req->to_forward || req->send_max))) {
		/* Output closed while we were sending data. We must abort. */
		buffer_ignore(req, req->l - req->send_max);
		buffer_auto_read(req);
		buffer_auto_close(req);
		req->analysers &= ~an_bit;
		return 1;
	}

	buffer_dont_close(req);

	/* Note that we don't have to send 100-continue back because we don't
	 * need the data to complete our job, and it's up to the server to
	 * decide whether to return 100, 417 or anything else in return of
	 * an "Expect: 100-continue" header.
	 */

	if (msg->msg_state < HTTP_MSG_CHUNK_SIZE) {
		/* we have msg->sov which points to the first
		 * byte of message body. msg->som still points to the beginning
		 * of the message. We must save the body in req->lr because it
		 * survives buffer re-alignments.
		 */
		req->lr = req->data + msg->sov;
		if (txn->flags & TX_REQ_TE_CHNK)
			msg->msg_state = HTTP_MSG_CHUNK_SIZE;
		else {
			msg->msg_state = HTTP_MSG_DATA;
		}
	}

	while (1) {
		int bytes;

		http_silent_debug(__LINE__, s);
		/* we may have some data pending */
		bytes = msg->sov - msg->som;
		if (msg->chunk_len || bytes) {
			msg->som = msg->sov;
			if (likely(bytes < 0)) /* sov may have wrapped at the end */
				bytes += req->size;
			msg->chunk_len += (unsigned int)bytes;
			msg->chunk_len -= buffer_forward(req, msg->chunk_len);
		}

		if (msg->msg_state == HTTP_MSG_DATA) {
			/* must still forward */
			if (req->to_forward)
				goto missing_data;

			/* nothing left to forward */
			if (txn->flags & TX_REQ_TE_CHNK)
				msg->msg_state = HTTP_MSG_DATA_CRLF;
			else
				msg->msg_state = HTTP_MSG_DONE;
		}
		else if (msg->msg_state == HTTP_MSG_CHUNK_SIZE) {
			/* read the chunk size and assign it to ->chunk_len, then
			 * set ->sov and ->lr to point to the body and switch to DATA or
			 * TRAILERS state.
			 */
			int ret = http_parse_chunk_size(req, msg);

			if (!ret)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->fe->invalid_req, s, req, msg, HTTP_MSG_CHUNK_SIZE, s->be);
				goto return_bad_req;
			}
			/* otherwise we're in HTTP_MSG_DATA or HTTP_MSG_TRAILERS state */
		}
		else if (msg->msg_state == HTTP_MSG_DATA_CRLF) {
			/* we want the CRLF after the data */
			int ret;

			req->lr = req->w + req->send_max;
			if (req->lr >= req->data + req->size)
				req->lr -= req->size;

			ret = http_skip_chunk_crlf(req, msg);

			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->fe->invalid_req, s, req, msg, HTTP_MSG_DATA_CRLF, s->be);
				goto return_bad_req;
			}
			/* we're in MSG_CHUNK_SIZE now */
		}
		else if (msg->msg_state == HTTP_MSG_TRAILERS) {
			int ret = http_forward_trailers(req, msg);

			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->fe->invalid_req, s, req, msg, HTTP_MSG_TRAILERS, s->be);
				goto return_bad_req;
			}
			/* we're in HTTP_MSG_DONE now */
		}
		else {
			int old_state = msg->msg_state;

			/* other states, DONE...TUNNEL */
			if (http_resync_states(s)) {
				/* some state changes occurred, maybe the analyser
				 * was disabled too.
				 */
				if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
					if (req->flags & BF_SHUTW) {
						/* request errors are most likely due to
						 * the server aborting the transfer.
						 */
						goto aborted_xfer;
					}
					if (msg->err_pos >= 0)
						http_capture_bad_message(&s->fe->invalid_req, s, req, msg, old_state, s->be);
					goto return_bad_req;
				}
				return 1;
			}

			/* If "option abortonclose" is set on the backend, we
			 * want to monitor the client's connection and forward
			 * any shutdown notification to the server, which will
			 * decide whether to close or to go on processing the
			 * request.
			 */
			if (s->be->options & PR_O_ABRT_CLOSE) {
				buffer_auto_read(req);
				buffer_auto_close(req);
			}
			else if (s->txn.meth == HTTP_METH_POST) {
				/* POST requests may require to read extra CRLF
				 * sent by broken browsers and which could cause
				 * an RST to be sent upon close on some systems
				 * (eg: Linux).
				 */
				buffer_auto_read(req);
			}

			return 0;
		}
	}

 missing_data:
	/* stop waiting for data if the input is closed before the end */
	if (req->flags & BF_SHUTR) {
		if (!(s->flags & SN_ERR_MASK))
			s->flags |= SN_ERR_CLICL;
		if (!(s->flags & SN_FINST_MASK)) {
			if (txn->rsp.msg_state < HTTP_MSG_ERROR)
				s->flags |= SN_FINST_H;
			else
				s->flags |= SN_FINST_D;
		}

		s->fe->counters.cli_aborts++;
		if (s->fe != s->be)
			s->be->counters.cli_aborts++;
		if (s->srv)
			s->srv->counters.cli_aborts++;

		goto return_bad_req_stats_ok;
	}

	/* waiting for the last bits to leave the buffer */
	if (req->flags & BF_SHUTW)
		goto aborted_xfer;

	/* We know that more data are expected, but we couldn't send more that
	 * what we did. So we always set the BF_EXPECT_MORE flag so that the
	 * system knows it must not set a PUSH on this first part. Interactive
	 * modes are already handled by the stream sock layer. We must not do
	 * this in content-length mode because it could present the MSG_MORE
	 * flag with the last block of forwarded data, which would cause an
	 * additional delay to be observed by the receiver.
	 */
	if (txn->flags & TX_REQ_TE_CHNK)
		req->flags |= BF_EXPECT_MORE;

	http_silent_debug(__LINE__, s);
	return 0;

 return_bad_req: /* let's centralize all bad requests */
	s->fe->counters.failed_req++;
	if (s->listener->counters)
		s->listener->counters->failed_req++;
 return_bad_req_stats_ok:
	txn->req.msg_state = HTTP_MSG_ERROR;
	if (txn->status) {
		/* Note: we don't send any error if some data were already sent */
		stream_int_retnclose(req->prod, NULL);
	} else {
		txn->status = 400;
		stream_int_retnclose(req->prod, error_message(s, HTTP_ERR_400));
	}
	req->analysers = 0;
	s->rep->analysers = 0; /* we're in data phase, we want to abort both directions */

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK)) {
		if (txn->rsp.msg_state < HTTP_MSG_ERROR)
			s->flags |= SN_FINST_H;
		else
			s->flags |= SN_FINST_D;
	}
	return 0;

 aborted_xfer:
	txn->req.msg_state = HTTP_MSG_ERROR;
	if (txn->status) {
		/* Note: we don't send any error if some data were already sent */
		stream_int_retnclose(req->prod, NULL);
	} else {
		txn->status = 502;
		stream_int_retnclose(req->prod, error_message(s, HTTP_ERR_502));
	}
	req->analysers = 0;
	s->rep->analysers = 0; /* we're in data phase, we want to abort both directions */

	s->fe->counters.srv_aborts++;
	if (s->fe != s->be)
		s->be->counters.srv_aborts++;
	if (s->srv)
		s->srv->counters.srv_aborts++;

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_SRVCL;
	if (!(s->flags & SN_FINST_MASK)) {
		if (txn->rsp.msg_state < HTTP_MSG_ERROR)
			s->flags |= SN_FINST_H;
		else
			s->flags |= SN_FINST_D;
	}
	return 0;
}