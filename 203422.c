int http_sync_req_state(struct session *s)
{
	struct buffer *buf = s->req;
	struct http_txn *txn = &s->txn;
	unsigned int old_flags = buf->flags;
	unsigned int old_state = txn->req.msg_state;

	http_silent_debug(__LINE__, s);
	if (unlikely(txn->req.msg_state < HTTP_MSG_BODY))
		return 0;

	if (txn->req.msg_state == HTTP_MSG_DONE) {
		/* No need to read anymore, the request was completely parsed.
		 * We can shut the read side unless we want to abort_on_close,
		 * or we have a POST request. The issue with POST requests is
		 * that some browsers still send a CRLF after the request, and
		 * this CRLF must be read so that it does not remain in the kernel
		 * buffers, otherwise a close could cause an RST on some systems
		 * (eg: Linux).
		 */
		if (!(s->be->options & PR_O_ABRT_CLOSE) && txn->meth != HTTP_METH_POST)
			buffer_dont_read(buf);

		if (txn->rsp.msg_state == HTTP_MSG_ERROR)
			goto wait_other_side;

		if (txn->rsp.msg_state < HTTP_MSG_DONE) {
			/* The server has not finished to respond, so we
			 * don't want to move in order not to upset it.
			 */
			goto wait_other_side;
		}

		if (txn->rsp.msg_state == HTTP_MSG_TUNNEL) {
			/* if any side switches to tunnel mode, the other one does too */
			buffer_auto_read(buf);
			txn->req.msg_state = HTTP_MSG_TUNNEL;
			buf->flags |= BF_NEVER_WAIT;
			goto wait_other_side;
		}

		/* When we get here, it means that both the request and the
		 * response have finished receiving. Depending on the connection
		 * mode, we'll have to wait for the last bytes to leave in either
		 * direction, and sometimes for a close to be effective.
		 */

		if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL) {
			/* Server-close mode : queue a connection close to the server */
			if (!(buf->flags & (BF_SHUTW|BF_SHUTW_NOW)))
				buffer_shutw_now(buf);
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
			txn->req.msg_state = HTTP_MSG_TUNNEL;
			buf->flags |= BF_NEVER_WAIT;
		}

		if (buf->flags & (BF_SHUTW|BF_SHUTW_NOW)) {
			/* if we've just closed an output, let's switch */
			buf->cons->flags |= SI_FL_NOLINGER;  /* we want to close ASAP */

			if (!(buf->flags & BF_OUT_EMPTY)) {
				txn->req.msg_state = HTTP_MSG_CLOSING;
				goto http_msg_closing;
			}
			else {
				txn->req.msg_state = HTTP_MSG_CLOSED;
				goto http_msg_closed;
			}
		}
		goto wait_other_side;
	}

	if (txn->req.msg_state == HTTP_MSG_CLOSING) {
	http_msg_closing:
		/* nothing else to forward, just waiting for the output buffer
		 * to be empty and for the shutw_now to take effect.
		 */
		if (buf->flags & BF_OUT_EMPTY) {
			txn->req.msg_state = HTTP_MSG_CLOSED;
			goto http_msg_closed;
		}
		else if (buf->flags & BF_SHUTW) {
			txn->req.msg_state = HTTP_MSG_ERROR;
			goto wait_other_side;
		}
	}

	if (txn->req.msg_state == HTTP_MSG_CLOSED) {
	http_msg_closed:
		goto wait_other_side;
	}

 wait_other_side:
	http_silent_debug(__LINE__, s);
	return txn->req.msg_state != old_state || buf->flags != old_flags;
}