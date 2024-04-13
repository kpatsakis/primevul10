void http_capture_bad_message(struct error_snapshot *es, struct session *s,
                              struct buffer *buf, struct http_msg *msg,
			      int state, struct proxy *other_end)
{
	if (buf->r <= (buf->data + msg->som)) { /* message wraps */
		int len1 = buf->size - msg->som;
		es->len = buf->r - (buf->data + msg->som) + buf->size;
		memcpy(es->buf, buf->data + msg->som, MIN(len1, sizeof(es->buf)));
		if (es->len > len1 && len1 < sizeof(es->buf))
			memcpy(es->buf + len1, buf->data, MIN(es->len, sizeof(es->buf)) - len1);
	}
	else {
		es->len = buf->r - (buf->data + msg->som);
		memcpy(es->buf, buf->data + msg->som, MIN(es->len, sizeof(es->buf)));
	}

	if (msg->err_pos >= 0)
		es->pos  = msg->err_pos - msg->som;
	else if (buf->lr >= (buf->data + msg->som))
		es->pos  = buf->lr - (buf->data + msg->som);
	else
		es->pos  = buf->lr - (buf->data + msg->som) + buf->size;

	es->when = date; // user-visible date
	es->sid  = s->uniq_id;
	es->srv  = s->srv;
	es->oe   = other_end;
	es->src  = s->cli_addr;
	es->state = state;
	es->flags = buf->flags;
	es->ev_id = error_snapshot_id++;
}