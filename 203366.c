int http_sync_res_state(struct session *s)
{
	struct buffer *buf = s->rep;
	struct http_txn *txn = &s->txn;
	unsigned int old_flags = buf->flags;
	unsigned int old_state = txn->rsp.msg_state;

	http_silent_debug(__LINE__, s);
	if (unlikely(txn->rsp.msg_state < HTTP_MSG_BODY))
		return 0;

	if (txn->rsp.msg_state == HTTP_MSG_DONE) {
		/* In theory, we don't need to read anymore, but we must
		 * still monitor the server connection for a possible close
		 * while the request is being uploaded, so we don't disable
		 * reading.
		 */
		/* buffer_dont_read(buf); */

		if (txn->req.msg_state == HTTP_MSG_ERROR)
			goto wait_other_side;

		if (txn->req.msg_state < HTTP_MSG_DONE) {
			/* The client seems to still be sending data, probably
			 * because we got an error response during an upload.
			 * We have the choice of either breaking the connection
			 * or letting it pass through. Let's do the later.
			 */
			goto wait_other_side;
		}

		if (txn->req.msg_state == HTTP_MSG_TUNNEL) {
			/* if any side switches to tunnel mode, the other one does too */
			buffer_auto_read(buf);
			txn->rsp.msg_state = HTTP_MSG_TUNNEL;
			buf->flags |= BF_NEVER_WAIT;
			goto wait_other_side;
		}

		/* When we get here, it means that both the request and the
		 * response have finished receiving. Depending on the connection
		 * mode, we'll have to wait for the last bytes to leave in either
		 * direction, and sometimes for a close to be effective.
		 */

		if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL) {
			/* Server-close mode : shut read and wait for the request
			 * side to close its output buffer. The caller will detect
			 * when we're in DONE and the other is in CLOSED and will
			 * catch that for the final cleanup.
			 */
			if (!(buf->flags & (BF_SHUTR|BF_SHUTR_NOW)))
				buffer_shutr_now(buf);
		}
		else if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_CLO) {
			/* Option forceclose is set, or either side wants to close,
			 * let's enforce it now that we're not expecting any new
			 * data to come. The caller knows the session is complete
			 * once both states are CLOSED.
			 */
			if (!(buf->flags & (BF_SHUTW|BF_SHUTW_NOW))) {
				buffer_shutr_now(buf);
				buffer_shutw_now(buf);
			}
		}
		else {
			/* The last possible modes are keep-alive and tunnel. Since tunnel
			 * mode does not set the body analyser, we can't reach this place
			 * in tunnel mode, so we're left with keep-alive only.
			 * This mode is currently not implemented, we switch to tunnel mode.
			 */
			buffer_auto_read(buf);
			txn->rsp.msg_state = HTTP_MSG_TUNNEL;
			buf->flags |= BF_NEVER_WAIT;
		}

		if (buf->flags & (BF_SHUTW|BF_SHUTW_NOW)) {
			/* if we've just closed an output, let's switch */
			if (!(buf->flags & BF_OUT_EMPTY)) {
				txn->rsp.msg_state = HTTP_MSG_CLOSING;
				goto http_msg_closing;
			}
			else {
				txn->rsp.msg_state = HTTP_MSG_CLOSED;
				goto http_msg_closed;
			}
		}
		goto wait_other_side;
	}

	if (txn->rsp.msg_state == HTTP_MSG_CLOSING) {
	http_msg_closing:
		/* nothing else to forward, just waiting for the output buffer
		 * to be empty and for the shutw_now to take effect.
		 */
		if (buf->flags & BF_OUT_EMPTY) {
			txn->rsp.msg_state = HTTP_MSG_CLOSED;
			goto http_msg_closed;
		}
		else if (buf->flags & BF_SHUTW) {
			txn->rsp.msg_state = HTTP_MSG_ERROR;
			s->be->counters.cli_aborts++;
			if (s->srv)
				s->srv->counters.cli_aborts++;
			goto wait_other_side;
		}
	}

	if (txn->rsp.msg_state == HTTP_MSG_CLOSED) {
	http_msg_closed:
		/* drop any pending data */
		buffer_ignore(buf, buf->l - buf->send_max);
		buffer_auto_close(buf);
		buffer_auto_read(buf);
		goto wait_other_side;
	}

 wait_other_side:
	http_silent_debug(__LINE__, s);

	/* We force the response to leave immediately if we're waiting for the
	 * other side, since there is no pending shutdown to push it out.
	 */
	if (!(buf->flags & BF_OUT_EMPTY))
		buf->flags |= BF_SEND_DONTWAIT;
	return txn->rsp.msg_state != old_state || buf->flags != old_flags;
}