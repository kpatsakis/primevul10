void perform_http_redirect(struct session *s, struct stream_interface *si)
{
	struct http_txn *txn;
	struct chunk rdr;
	char *path;
	int len;

	/* 1: create the response header */
	rdr.len = strlen(HTTP_302);
	rdr.str = trash;
	rdr.size = trashlen;
	memcpy(rdr.str, HTTP_302, rdr.len);

	/* 2: add the server's prefix */
	if (rdr.len + s->srv->rdr_len > rdr.size)
		return;

	/* special prefix "/" means don't change URL */
	if (s->srv->rdr_len != 1 || *s->srv->rdr_pfx != '/') {
		memcpy(rdr.str + rdr.len, s->srv->rdr_pfx, s->srv->rdr_len);
		rdr.len += s->srv->rdr_len;
	}

	/* 3: add the request URI */
	txn = &s->txn;
	path = http_get_path(txn);
	if (!path)
		return;

	len = txn->req.sl.rq.u_l + (txn->req.sol + txn->req.sl.rq.u) - path;
	if (rdr.len + len > rdr.size - 4) /* 4 for CRLF-CRLF */
		return;

	memcpy(rdr.str + rdr.len, path, len);
	rdr.len += len;

	if (unlikely(txn->flags & TX_USE_PX_CONN)) {
		memcpy(rdr.str + rdr.len, "\r\nProxy-Connection: close\r\n\r\n", 29);
		rdr.len += 29;
	} else {
		memcpy(rdr.str + rdr.len, "\r\nConnection: close\r\n\r\n", 23);
		rdr.len += 23;
	}

	/* prepare to return without error. */
	si->shutr(si);
	si->shutw(si);
	si->err_type = SI_ET_NONE;
	si->err_loc  = NULL;
	si->state    = SI_ST_CLO;

	/* send the message */
	http_server_error(s, si, SN_ERR_PRXCOND, SN_FINST_C, 302, &rdr);

	/* FIXME: we should increase a counter of redirects per server and per backend. */
	srv_inc_sess_ctr(s->srv);
}