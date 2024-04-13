void http_sess_clflog(struct session *s)
{
	char pn[INET6_ADDRSTRLEN + strlen(":65535")];
	struct proxy *fe = s->fe;
	struct proxy *be = s->be;
	struct proxy *prx_log;
	struct http_txn *txn = &s->txn;
	int tolog, level, err;
	char *uri, *h;
	char *svid;
	struct tm tm;
	static char tmpline[MAX_SYSLOG_LEN];
	int hdr;
	size_t w;
	int t_request;

	prx_log = fe;
	err = (s->flags & (SN_ERR_MASK | SN_REDISP)) ||
		(s->conn_retries != be->conn_retries) ||
		txn->status >= 500;

	if (s->cli_addr.ss_family == AF_INET)
		inet_ntop(AF_INET,
		          (const void *)&((struct sockaddr_in *)&s->cli_addr)->sin_addr,
		          pn, sizeof(pn));
	else
		inet_ntop(AF_INET6,
		          (const void *)&((struct sockaddr_in6 *)(&s->cli_addr))->sin6_addr,
		          pn, sizeof(pn));

	get_gmtime(s->logs.accept_date.tv_sec, &tm);

	/* FIXME: let's limit ourselves to frontend logging for now. */
	tolog = fe->to_log;

	h = tmpline;

	w = snprintf(h, sizeof(tmpline),
	             "%s - - [%02d/%s/%04d:%02d:%02d:%02d +0000]",
	             pn,
	             tm.tm_mday, monthname[tm.tm_mon], tm.tm_year+1900,
	             tm.tm_hour, tm.tm_min, tm.tm_sec);
	if (w < 0 || w >= sizeof(tmpline) - (h - tmpline))
		goto trunc;
	h += w;

	if (h >= tmpline + sizeof(tmpline) - 4)
		goto trunc;

	*(h++) = ' ';
	*(h++) = '\"';
	uri = txn->uri ? txn->uri : "<BADREQ>";
	h = encode_string(h, tmpline + sizeof(tmpline) - 1,
	                  '#', url_encode_map, uri);
	*(h++) = '\"';

	w = snprintf(h, sizeof(tmpline) - (h - tmpline), " %d %lld", txn->status, s->logs.bytes_out);
	if (w < 0 || w >= sizeof(tmpline) - (h - tmpline))
		goto trunc;
	h += w;

	if (h >= tmpline + sizeof(tmpline) - 9)
		goto trunc;
	memcpy(h, " \"-\" \"-\"", 8);
	h += 8;

	w = snprintf(h, sizeof(tmpline) - (h - tmpline),
	             " %d %03d",
	             (s->cli_addr.ss_family == AF_INET) ?
	             ntohs(((struct sockaddr_in *)&s->cli_addr)->sin_port) :
	             ntohs(((struct sockaddr_in6 *)&s->cli_addr)->sin6_port),
	             (int)s->logs.accept_date.tv_usec/1000);
	if (w < 0 || w >= sizeof(tmpline) - (h - tmpline))
		goto trunc;
	h += w;

	w = strlen(fe->id);
	if (h >= tmpline + sizeof(tmpline) - 4 - w)
		goto trunc;
	*(h++) = ' ';
	*(h++) = '\"';
	memcpy(h, fe->id, w);
	h += w;
	*(h++) = '\"';

	w = strlen(be->id);
	if (h >= tmpline + sizeof(tmpline) - 4 - w)
		goto trunc;
	*(h++) = ' ';
	*(h++) = '\"';
	memcpy(h, be->id, w);
	h += w;
	*(h++) = '\"';

	svid = (tolog & LW_SVID) ?
		(s->data_source != DATA_SRC_STATS) ?
		(s->srv != NULL) ? s->srv->id : "<NOSRV>" : "<STATS>" : "-";

	w = strlen(svid);
	if (h >= tmpline + sizeof(tmpline) - 4 - w)
		goto trunc;
	*(h++) = ' ';
	*(h++) = '\"';
	memcpy(h, svid, w);
	h += w;
	*(h++) = '\"';

	t_request = -1;
	if (tv_isge(&s->logs.tv_request, &s->logs.tv_accept))
		t_request = tv_ms_elapsed(&s->logs.tv_accept, &s->logs.tv_request);
	w = snprintf(h, sizeof(tmpline) - (h - tmpline),
	             " %d %ld %ld %ld %ld",
	             t_request,
	             (s->logs.t_queue >= 0) ? s->logs.t_queue - t_request : -1,
	             (s->logs.t_connect >= 0) ? s->logs.t_connect - s->logs.t_queue : -1,
	             (s->logs.t_data >= 0) ? s->logs.t_data - s->logs.t_connect : -1,
	             s->logs.t_close);
	if (w < 0 || w >= sizeof(tmpline) - (h - tmpline))
		goto trunc;
	h += w;

	if (h >= tmpline + sizeof(tmpline) - 8)
		goto trunc;
	*(h++) = ' ';
	*(h++) = '\"';
	*(h++) = sess_term_cond[(s->flags & SN_ERR_MASK) >> SN_ERR_SHIFT];
	*(h++) = sess_fin_state[(s->flags & SN_FINST_MASK) >> SN_FINST_SHIFT];
	*(h++) = (be->ck_opts) ? sess_cookie[(txn->flags & TX_CK_MASK) >> TX_CK_SHIFT] : '-',
	*(h++) = (be->ck_opts) ? sess_set_cookie[(txn->flags & TX_SCK_MASK) >> TX_SCK_SHIFT] : '-';
	*(h++) = '\"';

	w = snprintf(h, sizeof(tmpline) - (h - tmpline),
	             " %d %d %d %d %d %ld %ld",
	             actconn, fe->feconn, be->beconn, s->srv ? s->srv->cur_sess : 0,
	             (s->conn_retries > 0) ? (be->conn_retries - s->conn_retries) : be->conn_retries,
	             s->logs.srv_queue_size, s->logs.prx_queue_size);

	if (w < 0 || w >= sizeof(tmpline) - (h - tmpline))
		goto trunc;
	h += w;

	if (txn->cli_cookie) {
		w = strlen(txn->cli_cookie);
		if (h >= tmpline + sizeof(tmpline) - 4 - w)
			goto trunc;
		*(h++) = ' ';
		*(h++) = '\"';
		memcpy(h, txn->cli_cookie, w);
		h += w;
		*(h++) = '\"';
	} else {
		if (h >= tmpline + sizeof(tmpline) - 5)
			goto trunc;
		memcpy(h, " \"-\"", 4);
		h += 4;
	}

	if (txn->srv_cookie) {
		w = strlen(txn->srv_cookie);
		if (h >= tmpline + sizeof(tmpline) - 4 - w)
			goto trunc;
		*(h++) = ' ';
		*(h++) = '\"';
		memcpy(h, txn->srv_cookie, w);
		h += w;
		*(h++) = '\"';
	} else {
		if (h >= tmpline + sizeof(tmpline) - 5)
			goto trunc;
		memcpy(h, " \"-\"", 4);
		h += 4;
	}

	if ((fe->to_log & LW_REQHDR) && txn->req.cap) {
		for (hdr = 0; hdr < fe->nb_req_cap; hdr++) {
			if (h >= sizeof (tmpline) + tmpline - 4)
				goto trunc;
			if (txn->req.cap[hdr] != NULL) {
				*(h++) = ' ';
				*(h++) = '\"';
				h = encode_string(h, tmpline + sizeof(tmpline) - 2,
						'#', hdr_encode_map, txn->req.cap[hdr]);
				*(h++) = '\"';
			} else {
				memcpy(h, " \"-\"", 4);
				h += 4;
			}
		}
	}

	if ((fe->to_log & LW_RSPHDR) && txn->rsp.cap) {
		for (hdr = 0; hdr < fe->nb_rsp_cap; hdr++) {
			if (h >= sizeof (tmpline) + tmpline - 4)
				goto trunc;
			if (txn->rsp.cap[hdr] != NULL) {
				*(h++) = ' ';
				*(h++) = '\"';
				h = encode_string(h, tmpline + sizeof(tmpline) - 2,
						'#', hdr_encode_map, txn->rsp.cap[hdr]);
				*(h++) = '\"';
			} else {
				memcpy(h, " \"-\"", 4);
				h += 4;
			}
		}
	}

trunc:
	*h = '\0';

	level = LOG_INFO;
	if (err && (fe->options2 & PR_O2_LOGERRORS))
		level = LOG_ERR;

	send_log(prx_log, level, "%s\n", tmpline);

	s->logs.logwait = 0;
}