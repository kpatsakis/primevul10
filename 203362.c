int http_skip_chunk_crlf(struct buffer *buf, struct http_msg *msg)
{
	char *ptr;
	int bytes;

	/* NB: we'll check data availabilty at the end. It's not a
	 * problem because whatever we match first will be checked
	 * against the correct length.
	 */
	bytes = 1;
	ptr = buf->lr;
	if (*ptr == '\r') {
		bytes++;
		ptr++;
		if (ptr >= buf->data + buf->size)
			ptr = buf->data;
	}

	if (bytes > buf->l - buf->send_max)
		return 0;

	if (*ptr != '\n') {
		msg->err_pos = ptr - buf->data;
		return -1;
	}

	ptr++;
	if (ptr >= buf->data + buf->size)
		ptr = buf->data;
	buf->lr = ptr;
	/* prepare the CRLF to be forwarded. msg->som may be before data but we don't care */
	msg->sov = ptr - buf->data;
	msg->som = msg->sov - bytes;
	msg->msg_state = HTTP_MSG_CHUNK_SIZE;
	return 1;
}