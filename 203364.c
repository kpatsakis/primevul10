int http_forward_trailers(struct buffer *buf, struct http_msg *msg)
{
	/* we have buf->lr which points to next line. Look for CRLF. */
	while (1) {
		char *p1 = NULL, *p2 = NULL;
		char *ptr = buf->lr;
		int bytes;

		/* scan current line and stop at LF or CRLF */
		while (1) {
			if (ptr == buf->r)
				return 0;

			if (*ptr == '\n') {
				if (!p1)
					p1 = ptr;
				p2 = ptr;
				break;
			}

			if (*ptr == '\r') {
				if (p1) {
					msg->err_pos = ptr - buf->data;
					return -1;
				}
				p1 = ptr;
			}

			ptr++;
			if (ptr >= buf->data + buf->size)
				ptr = buf->data;
		}

		/* after LF; point to beginning of next line */
		p2++;
		if (p2 >= buf->data + buf->size)
			p2 = buf->data;

		bytes = p2 - buf->lr;
		if (bytes < 0)
			bytes += buf->size;

		/* schedule this line for forwarding */
		msg->sov += bytes;
		if (msg->sov >= buf->size)
			msg->sov -= buf->size;

		if (p1 == buf->lr) {
			/* LF/CRLF at beginning of line => end of trailers at p2.
			 * Everything was scheduled for forwarding, there's nothing
			 * left from this message.
			 */
			buf->lr = p2;
			msg->msg_state = HTTP_MSG_DONE;
			return 1;
		}
		/* OK, next line then */
		buf->lr = p2;
	}
}