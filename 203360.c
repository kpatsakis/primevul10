int http_send_name_header(struct http_txn *txn, struct http_msg *msg, struct buffer *buf, struct proxy* be, const char* srv_name) {

	struct hdr_ctx ctx;

	ctx.idx = 0;

	char *hdr_name = be->server_id_hdr_name;
	int hdr_name_len = be->server_id_hdr_len;

	char *hdr_val;

	int delta = txn->req.eoh;
	while (http_find_header2(hdr_name, hdr_name_len, msg->sol, &txn->hdr_idx, &ctx)) {
		/* remove any existing values from the header */
	        http_remove_header2(msg, buf, &txn->hdr_idx, &ctx);
	}

	/* Add the new header requested with the server value */
	hdr_val = trash;
	memcpy(hdr_val, hdr_name, hdr_name_len);
	hdr_val += hdr_name_len;
	*hdr_val++ = ':';
	*hdr_val++ = ' ';
	hdr_val += strlcpy2(hdr_val, srv_name, trash + trashlen - hdr_val);
	http_header_add_tail2(buf, msg, &txn->hdr_idx, trash, hdr_val - trash);
	delta -= txn->req.eoh;

	/* Adjust buffer data length to send */
	buf->send_max -= delta;

	return 0;
}