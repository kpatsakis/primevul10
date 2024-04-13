void http_buffer_heavy_realign(struct buffer *buf, struct http_msg *msg)
{
	char *end = buf->data + buf->size;
	int off = buf->data + buf->size - buf->w;

	/* two possible cases :
	 *   - the buffer is in one contiguous block, we move it in-place
	 *   - the buffer is in two blocks, we move it via the swap_buffer
	 */
	if (buf->l) {
		int block1 = buf->l;
		int block2 = 0;
		if (buf->r <= buf->w) {
			/* non-contiguous block */
			block1 = buf->data + buf->size - buf->w;
			block2 = buf->r - buf->data;
		}
		if (block2)
			memcpy(swap_buffer, buf->data, block2);
		memmove(buf->data, buf->w, block1);
		if (block2)
			memcpy(buf->data + block1, swap_buffer, block2);
	}

	/* adjust all known pointers */
	buf->w    = buf->data;
	buf->lr  += off; if (buf->lr  >= end) buf->lr  -= buf->size;
	buf->r   += off; if (buf->r   >= end) buf->r   -= buf->size;
	msg->sol += off; if (msg->sol >= end) msg->sol -= buf->size;
	msg->eol += off; if (msg->eol >= end) msg->eol -= buf->size;

	/* adjust relative pointers */
	msg->som  = 0;
	msg->eoh += off; if (msg->eoh >= buf->size) msg->eoh -= buf->size;
	msg->sov += off; if (msg->sov >= buf->size) msg->sov -= buf->size;

	if (msg->err_pos >= 0) {
		msg->err_pos += off;
		if (msg->err_pos >= buf->size)
			msg->err_pos -= buf->size;
	}

	buf->flags &= ~BF_FULL;
	if (buf->l >= buffer_max_len(buf))
		buf->flags |= BF_FULL;
}