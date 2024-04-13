int http_response_forward_body(struct session *s, struct buffer *res, int an_bit)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &s->txn.rsp;
	int bytes;

	if (unlikely(msg->msg_state < HTTP_MSG_BODY))
		return 0;

	if ((res->flags & (BF_READ_ERROR|BF_READ_TIMEOUT|BF_WRITE_ERROR|BF_WRITE_TIMEOUT)) ||
	    ((res->flags & BF_SHUTW) && (res->to_forward || res->send_max)) ||
	    !s->req->analysers) {
		/* in case of error or if the other analyser went away, we can't analyse HTTP anymore */
		buffer_ignore(res, res->l - res->send_max);
		buffer_auto_read(res);
		buffer_auto_close(res);
		res->analysers &= ~an_bit;
		return 1;
	}

	buffer_dont_close(res);

	if (msg->msg_state < HTTP_MSG_CHUNK_SIZE) {
		/* we have msg->sov which both points to the first
		 * byte of message body. msg->som still points to the beginning
		 * of the message. We must save the body in req->lr because it
		 * survives buffer re-alignments.
		 */
		res->lr = res->data + msg->sov;
		if (txn->flags & TX_RES_TE_CHNK)
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
				bytes += res->size;
			msg->chunk_len += (unsigned int)bytes;
			msg->chunk_len -= buffer_forward(res, msg->chunk_len);
		}


		if (msg->msg_state == HTTP_MSG_DATA) {
			/* must still forward */
			if (res->to_forward)
				goto missing_data;

			/* nothing left to forward */
			if (txn->flags & TX_RES_TE_CHNK)
				msg->msg_state = HTTP_MSG_DATA_CRLF;
			else
				msg->msg_state = HTTP_MSG_DONE;
		}
		else if (msg->msg_state == HTTP_MSG_CHUNK_SIZE) {
			/* read the chunk size and assign it to ->chunk_len, then
			 * set ->sov to point to the body and switch to DATA or TRAILERS state.
			 */
			int ret = http_parse_chunk_size(res, msg);

			if (!ret)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->be->invalid_rep, s, res, msg, HTTP_MSG_CHUNK_SIZE, s->fe);
				goto return_bad_res;
			}
			/* otherwise we're in HTTP_MSG_DATA or HTTP_MSG_TRAILERS state */
		}
		else if (msg->msg_state == HTTP_MSG_DATA_CRLF) {
			/* we want the CRLF after the data */
			int ret;

			res->lr = res->w + res->send_max;
			if (res->lr >= res->data + res->size)
				res->lr -= res->size;

			ret = http_skip_chunk_crlf(res, msg);

			if (!ret)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->be->invalid_rep, s, res, msg, HTTP_MSG_DATA_CRLF, s->fe);
				goto return_bad_res;
			}
			/* we're in MSG_CHUNK_SIZE now */
		}
		else if (msg->msg_state == HTTP_MSG_TRAILERS) {
			int ret = http_forward_trailers(res, msg);

			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->be->invalid_rep, s, res, msg, HTTP_MSG_TRAILERS, s->fe);
				goto return_bad_res;
			}
			/* we're in HTTP_MSG_DONE now */
		}
		else {
			int old_state = msg->msg_state;

			/* other states, DONE...TUNNEL */
			if (http_resync_states(s)) {
				http_silent_debug(__LINE__, s);
				/* some state changes occurred, maybe the analyser
				 * was disabled too.
				 */
				if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
					if (res->flags & BF_SHUTW) {
						/* response errors are most likely due to
						 * the client aborting the transfer.
						 */
						goto aborted_xfer;
					}
					if (msg->err_pos >= 0)
						http_capture_bad_message(&s->be->invalid_rep, s, res, msg, old_state, s->fe);
					goto return_bad_res;
				}
				return 1;
			}
			return 0;
		}
	}

 missing_data:

	if (res->flags & BF_SHUTW)
		goto aborted_xfer;

	/* stop waiting for data if the input is closed before the end. If the
	 * client side was already closed, it means that the client has aborted,
	 * so we don't want to count this as a server abort. Otherwise it's a
	 * server abort.
	 */
	if (res->flags & BF_SHUTR) {
		if ((res->flags & BF_SHUTW_NOW) || (s->req->flags & BF_SHUTR))
			goto aborted_xfer;
		if (!(s->flags & SN_ERR_MASK))
			s->flags |= SN_ERR_SRVCL;
		s->be->counters.srv_aborts++;
		if (s->srv)
			s->srv->counters.srv_aborts++;
		goto return_bad_res_stats_ok;
	}

	/* we need to obey the req analyser, so if it leaves, we must too */
	if (!s->req->analysers)
		goto return_bad_res;

	/* forward any pending data */
	bytes = msg->sov - msg->som;
	if (msg->chunk_len || bytes) {
		msg->som = msg->sov;
		if (likely(bytes < 0)) /* sov may have wrapped at the end */
			bytes += res->size;
		msg->chunk_len += (unsigned int)bytes;
		msg->chunk_len -= buffer_forward(res, msg->chunk_len);
	}

	/* We know that more data are expected, but we couldn't send more that
	 * what we did. So we always set the BF_EXPECT_MORE flag so that the
	 * system knows it must not set a PUSH on this first part. Interactive
	 * modes are already handled by the stream sock layer. We must not do
	 * this in content-length mode because it could present the MSG_MORE
	 * flag with the last block of forwarded data, which would cause an
	 * additional delay to be observed by the receiver.
	 */
	if (txn->flags & TX_RES_TE_CHNK)
		res->flags |= BF_EXPECT_MORE;

	/* the session handler will take care of timeouts and errors */
	http_silent_debug(__LINE__, s);
	return 0;

 return_bad_res: /* let's centralize all bad responses */
	s->be->counters.failed_resp++;
	if (s->srv)
		s->srv->counters.failed_resp++;

 return_bad_res_stats_ok:
	txn->rsp.msg_state = HTTP_MSG_ERROR;
	/* don't send any error message as we're in the body */
	stream_int_retnclose(res->cons, NULL);
	res->analysers = 0;
	s->req->analysers = 0; /* we're in data phase, we want to abort both directions */
	if (s->srv)
		health_adjust(s->srv, HANA_STATUS_HTTP_HDRRSP);

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_D;
	return 0;

 aborted_xfer:
	txn->rsp.msg_state = HTTP_MSG_ERROR;
	/* don't send any error message as we're in the body */
	stream_int_retnclose(res->cons, NULL);
	res->analysers = 0;
	s->req->analysers = 0; /* we're in data phase, we want to abort both directions */

	s->fe->counters.cli_aborts++;
	if (s->fe != s->be)
		s->be->counters.cli_aborts++;
	if (s->srv)
		s->srv->counters.cli_aborts++;

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_CLICL;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_D;
	return 0;
}