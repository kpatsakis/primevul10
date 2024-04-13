void http_sess_log(struct session *s)
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
	int t_request;
	int hdr;

	/* if we don't want to log normal traffic, return now */
	err = (s->flags & (SN_ERR_MASK | SN_REDISP)) ||
		(s->conn_retries != be->conn_retries) ||
		txn->status >= 500;
	if (!err && (fe->options2 & PR_O2_NOLOGNORM))
		return;

	if (fe->logfac1 < 0 && fe->logfac2 < 0)
		return;
	prx_log = fe;

	if (prx_log->options2 & PR_O2_CLFLOG)
		return http_sess_clflog(s);

	if (s->cli_addr.ss_family == AF_INET)
		inet_ntop(AF_INET,
			  (const void *)&((struct sockaddr_in *)&s->cli_addr)->sin_addr,
			  pn, sizeof(pn));
	else
		inet_ntop(AF_INET6,
			  (const void *)&((struct sockaddr_in6 *)(&s->cli_addr))->sin6_addr,
			  pn, sizeof(pn));

	get_localtime(s->logs.accept_date.tv_sec, &tm);

	/* FIXME: let's limit ourselves to frontend logging for now. */
	tolog = fe->to_log;

	h = tmpline;
	if (fe->to_log & LW_REQHDR &&
	    txn->req.cap &&
	    (h < tmpline + sizeof(tmpline) - 10)) {
		*(h++) = ' ';
		*(h++) = '{';
		for (hdr = 0; hdr < fe->nb_req_cap; hdr++) {
			if (hdr)
				*(h++) = '|';
			if (txn->req.cap[hdr] != NULL)
				h = encode_string(h, tmpline + sizeof(tmpline) - 7,
						  '#', hdr_encode_map, txn->req.cap[hdr]);
		}
		*(h++) = '}';
	}

	if (fe->to_log & LW_RSPHDR &&
	    txn->rsp.cap &&
	    (h < tmpline + sizeof(tmpline) - 7)) {
		*(h++) = ' ';
		*(h++) = '{';
		for (hdr = 0; hdr < fe->nb_rsp_cap; hdr++) {
			if (hdr)
				*(h++) = '|';
			if (txn->rsp.cap[hdr] != NULL)
				h = encode_string(h, tmpline + sizeof(tmpline) - 4,
						  '#', hdr_encode_map, txn->rsp.cap[hdr]);
		}
		*(h++) = '}';
	}

	if (h < tmpline + sizeof(tmpline) - 4) {
		*(h++) = ' ';
		*(h++) = '"';
		uri = txn->uri ? txn->uri : "<BADREQ>";
		h = encode_string(h, tmpline + sizeof(tmpline) - 1,
				  '#', url_encode_map, uri);
		*(h++) = '"';
	}
	*h = '\0';

	svid = (tolog & LW_SVID) ?
		(s->data_source != DATA_SRC_STATS) ?
		(s->srv != NULL) ? s->srv->id : "<NOSRV>" : "<STATS>" : "-";

	t_request = -1;
	if (tv_isge(&s->logs.tv_request, &s->logs.tv_accept))
		t_request = tv_ms_elapsed(&s->logs.tv_accept, &s->logs.tv_request);

	level = LOG_INFO;
	if (err && (fe->options2 & PR_O2_LOGERRORS))
		level = LOG_ERR;

	send_log(prx_log, level,
		 "%s:%d [%02d/%s/%04d:%02d:%02d:%02d.%03d]"
		 " %s %s/%s %d/%ld/%ld/%ld/%s%ld %d %s%lld"
		 " %s %s %c%c%c%c %d/%d/%d/%d/%s%u %ld/%ld%s\n",
		 pn,
		 (s->cli_addr.ss_family == AF_INET) ?
		 ntohs(((struct sockaddr_in *)&s->cli_addr)->sin_port) :
		 ntohs(((struct sockaddr_in6 *)&s->cli_addr)->sin6_port),
		 tm.tm_mday, monthname[tm.tm_mon], tm.tm_year+1900,
		 tm.tm_hour, tm.tm_min, tm.tm_sec, (int)s->logs.accept_date.tv_usec/1000,
		 fe->id, be->id, svid,
		 t_request,
		 (s->logs.t_queue >= 0) ? s->logs.t_queue - t_request : -1,
		 (s->logs.t_connect >= 0) ? s->logs.t_connect - s->logs.t_queue : -1,
		 (s->logs.t_data >= 0) ? s->logs.t_data - s->logs.t_connect : -1,
		 (tolog & LW_BYTES) ? "" : "+", s->logs.t_close,
		 txn->status,
		 (tolog & LW_BYTES) ? "" : "+", s->logs.bytes_out,
		 txn->cli_cookie ? txn->cli_cookie : "-",
		 txn->srv_cookie ? txn->srv_cookie : "-",
		 sess_term_cond[(s->flags & SN_ERR_MASK) >> SN_ERR_SHIFT],
		 sess_fin_state[(s->flags & SN_FINST_MASK) >> SN_FINST_SHIFT],
		 (be->ck_opts) ? sess_cookie[(txn->flags & TX_CK_MASK) >> TX_CK_SHIFT] : '-',
		 (be->ck_opts) ? sess_set_cookie[(txn->flags & TX_SCK_MASK) >> TX_SCK_SHIFT] : '-',
		 actconn, fe->feconn, be->beconn, s->srv ? s->srv->cur_sess : 0,
		 (s->flags & SN_REDISP)?"+":"",
		 (s->conn_retries>0)?(be->conn_retries - s->conn_retries):be->conn_retries,
		 s->logs.srv_queue_size, s->logs.prx_queue_size, tmpline);

	s->logs.logwait = 0;
}