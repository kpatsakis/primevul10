int stats_check_uri(struct session *t, struct proxy *backend)
{
	struct http_txn *txn = &t->txn;
	struct uri_auth *uri_auth = backend->uri_auth;
	char *h;

	if (!uri_auth)
		return 0;

	if (txn->meth != HTTP_METH_GET && txn->meth != HTTP_METH_HEAD && txn->meth != HTTP_METH_POST)
		return 0;

	memset(&t->data_ctx.stats, 0, sizeof(t->data_ctx.stats));

	/* check URI size */
	if (uri_auth->uri_len > txn->req.sl.rq.u_l)
		return 0;

	h = txn->req.sol + txn->req.sl.rq.u;

	/* the URI is in h */
	if (memcmp(h, uri_auth->uri_prefix, uri_auth->uri_len) != 0)
		return 0;

	h += uri_auth->uri_len;
	while (h <= txn->req.sol + txn->req.sl.rq.u + txn->req.sl.rq.u_l - 3) {
		if (memcmp(h, ";up", 3) == 0) {
			t->data_ctx.stats.flags |= STAT_HIDE_DOWN;
			break;
		}
		h++;
	}

	if (uri_auth->refresh) {
		h = txn->req.sol + txn->req.sl.rq.u + uri_auth->uri_len;
		while (h <= txn->req.sol + txn->req.sl.rq.u + txn->req.sl.rq.u_l - 10) {
			if (memcmp(h, ";norefresh", 10) == 0) {
				t->data_ctx.stats.flags |= STAT_NO_REFRESH;
				break;
			}
			h++;
		}
	}

	h = txn->req.sol + txn->req.sl.rq.u + uri_auth->uri_len;
	while (h <= txn->req.sol + txn->req.sl.rq.u + txn->req.sl.rq.u_l - 4) {
		if (memcmp(h, ";csv", 4) == 0) {
			t->data_ctx.stats.flags |= STAT_FMT_CSV;
			break;
		}
		h++;
	}

	h = txn->req.sol + txn->req.sl.rq.u + uri_auth->uri_len;
	while (h <= txn->req.sol + txn->req.sl.rq.u + txn->req.sl.rq.u_l - 8) {
		if (memcmp(h, ";st=", 4) == 0) {
			h += 4;

			if (memcmp(h, STAT_STATUS_DONE, 4) == 0)
				t->data_ctx.stats.st_code = STAT_STATUS_DONE;
			else if (memcmp(h, STAT_STATUS_NONE, 4) == 0)
				t->data_ctx.stats.st_code = STAT_STATUS_NONE;
			else if (memcmp(h, STAT_STATUS_PART, 4) == 0)
				t->data_ctx.stats.st_code = STAT_STATUS_PART;
			else if (memcmp(h, STAT_STATUS_ERRP, 4) == 0)
				t->data_ctx.stats.st_code = STAT_STATUS_ERRP;
			else if (memcmp(h, STAT_STATUS_EXCD, 4) == 0)
				t->data_ctx.stats.st_code = STAT_STATUS_EXCD;
			else if (memcmp(h, STAT_STATUS_DENY, 4) == 0)
				t->data_ctx.stats.st_code = STAT_STATUS_DENY;
			else
				t->data_ctx.stats.st_code = STAT_STATUS_UNKN;
			break;
		}
		h++;
	}

	t->data_ctx.stats.flags |= STAT_SHOW_STAT | STAT_SHOW_INFO;

	return 1;
}