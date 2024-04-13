int http_process_req_common(struct session *s, struct buffer *req, int an_bit, struct proxy *px)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->req;
	struct acl_cond *cond;
	struct req_acl_rule *req_acl, *req_acl_final = NULL;
	struct redirect_rule *rule;
	struct cond_wordlist *wl;
	int do_stats;

	if (unlikely(msg->msg_state < HTTP_MSG_BODY)) {
		/* we need more data */
		buffer_dont_connect(req);
		return 0;
	}

	req->analysers &= ~an_bit;
	req->analyse_exp = TICK_ETERNITY;

	DPRINTF(stderr,"[%u] %s: session=%p b=%p, exp(r,w)=%u,%u bf=%08x bl=%d analysers=%02x\n",
		now_ms, __FUNCTION__,
		s,
		req,
		req->rex, req->wex,
		req->flags,
		req->l,
		req->analysers);

	/* first check whether we have some ACLs set to block this request */
	list_for_each_entry(cond, &px->block_cond, list) {
		int ret = acl_exec_cond(cond, px, s, txn, ACL_DIR_REQ);

		ret = acl_pass(ret);
		if (cond->pol == ACL_COND_UNLESS)
			ret = !ret;

		if (ret) {
			txn->status = 403;
			/* let's log the request time */
			s->logs.tv_request = now;
			stream_int_retnclose(req->prod, error_message(s, HTTP_ERR_403));
			goto return_prx_cond;
		}
	}

	do_stats = stats_check_uri(s, px);

	list_for_each_entry(req_acl, (do_stats?&px->uri_auth->req_acl:&px->req_acl), list) {
		int ret = 1;

		if (req_acl->action >= PR_REQ_ACL_ACT_MAX)
			continue;

		/* check condition, but only if attached */
		if (req_acl->cond) {
			ret = acl_exec_cond(req_acl->cond, px, s, txn, ACL_DIR_REQ);
			ret = acl_pass(ret);

			if (req_acl->cond->pol == ACL_COND_UNLESS)
				ret = !ret;
		}

		if (ret) {
			req_acl_final = req_acl;
			break;
		}
	}

	if (req_acl_final && req_acl_final->action == PR_REQ_ACL_ACT_DENY) {
			txn->status = 403;
			s->logs.tv_request = now;
			stream_int_retnclose(req->prod, error_message(s, HTTP_ERR_403));
			goto return_prx_cond;
	}

	/* try headers filters */
	if (px->req_exp != NULL) {
		if (apply_filters_to_request(s, req, px) < 0)
			goto return_bad_req;

		/* has the request been denied ? */
		if (txn->flags & TX_CLDENY) {
			/* no need to go further */
			txn->status = 403;
			/* let's log the request time */
			s->logs.tv_request = now;
			stream_int_retnclose(req->prod, error_message(s, HTTP_ERR_403));
			goto return_prx_cond;
		}

		/* When a connection is tarpitted, we use the tarpit timeout,
		 * which may be the same as the connect timeout if unspecified.
		 * If unset, then set it to zero because we really want it to
		 * eventually expire. We build the tarpit as an analyser.
		 */
		if (txn->flags & TX_CLTARPIT) {
			buffer_erase(s->req);
			/* wipe the request out so that we can drop the connection early
			 * if the client closes first.
			 */
			buffer_dont_connect(req);
			req->analysers = 0; /* remove switching rules etc... */
			req->analysers |= AN_REQ_HTTP_TARPIT;
			req->analyse_exp = tick_add_ifset(now_ms,  s->be->timeout.tarpit);
			if (!req->analyse_exp)
				req->analyse_exp = tick_add(now_ms, 0);
			return 1;
		}
	}

	/* Until set to anything else, the connection mode is set as TUNNEL. It will
	 * only change if both the request and the config reference something else.
	 * Option httpclose by itself does not set a mode, it remains a tunnel mode
	 * in which headers are mangled. However, if another mode is set, it will
	 * affect it (eg: server-close/keep-alive + httpclose = close). Note that we
	 * avoid to redo the same work if FE and BE have the same settings (common).
	 * The method consists in checking if options changed between the two calls
	 * (implying that either one is non-null, or one of them is non-null and we
	 * are there for the first time.
	 */

	if ((!(txn->flags & TX_HDR_CONN_PRS) &&
	     (s->fe->options & (PR_O_KEEPALIVE|PR_O_SERVER_CLO|PR_O_HTTP_CLOSE|PR_O_FORCE_CLO))) ||
	    ((s->fe->options & (PR_O_KEEPALIVE|PR_O_SERVER_CLO|PR_O_HTTP_CLOSE|PR_O_FORCE_CLO)) !=
	     (s->be->options & (PR_O_KEEPALIVE|PR_O_SERVER_CLO|PR_O_HTTP_CLOSE|PR_O_FORCE_CLO)))) {
		int tmp = TX_CON_WANT_TUN;

		if ((s->fe->options|s->be->options) & PR_O_KEEPALIVE ||
		    ((s->fe->options2|s->be->options2) & PR_O2_FAKE_KA))
			tmp = TX_CON_WANT_KAL;
		if ((s->fe->options|s->be->options) & PR_O_SERVER_CLO)
			tmp = TX_CON_WANT_SCL;
		if ((s->fe->options|s->be->options) & PR_O_FORCE_CLO)
			tmp = TX_CON_WANT_CLO;

		if ((txn->flags & TX_CON_WANT_MSK) < tmp)
			txn->flags = (txn->flags & ~TX_CON_WANT_MSK) | tmp;

		if (!(txn->flags & TX_HDR_CONN_PRS)) {
			/* parse the Connection header and possibly clean it */
			int to_del = 0;
			if ((txn->flags & TX_REQ_VER_11) ||
			    ((txn->flags & TX_CON_WANT_MSK) >= TX_CON_WANT_SCL &&
			     !((s->fe->options2|s->be->options2) & PR_O2_FAKE_KA)))
				to_del |= 2; /* remove "keep-alive" */
			if (!(txn->flags & TX_REQ_VER_11))
				to_del |= 1; /* remove "close" */
			http_parse_connection_header(txn, msg, req, to_del);
		}

		/* check if client or config asks for explicit close in KAL/SCL */
		if (((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL ||
		     (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL) &&
		    ((txn->flags & TX_HDR_CONN_CLO) ||                         /* "connection: close" */
		     (txn->flags & (TX_REQ_VER_11|TX_HDR_CONN_KAL)) == 0 ||    /* no "connection: k-a" in 1.0 */
		     ((s->fe->options|s->be->options) & PR_O_HTTP_CLOSE) ||    /* httpclose+any = forceclose */
		     !(txn->flags & TX_REQ_XFER_LEN) ||                        /* no length known => close */
		     s->fe->state == PR_STSTOPPED))                            /* frontend is stopping */
		    txn->flags = (txn->flags & ~TX_CON_WANT_MSK) | TX_CON_WANT_CLO;
	}

	/* add request headers from the rule sets in the same order */
	list_for_each_entry(wl, &px->req_add, list) {
		if (wl->cond) {
			int ret = acl_exec_cond(wl->cond, px, s, txn, ACL_DIR_REQ);
			ret = acl_pass(ret);
			if (((struct acl_cond *)wl->cond)->pol == ACL_COND_UNLESS)
				ret = !ret;
			if (!ret)
				continue;
		}

		if (unlikely(http_header_add_tail(req, &txn->req, &txn->hdr_idx, wl->s) < 0))
			goto return_bad_req;
	}

	if (req_acl_final && req_acl_final->action == PR_REQ_ACL_ACT_HTTP_AUTH) {
		struct chunk msg;
		char *realm = req_acl_final->http_auth.realm;

		if (!realm)
			realm = do_stats?STATS_DEFAULT_REALM:px->id;

		sprintf(trash, (txn->flags & TX_USE_PX_CONN) ? HTTP_407_fmt : HTTP_401_fmt, realm);
		chunk_initlen(&msg, trash, trashlen, strlen(trash));
		txn->status = 401;
		stream_int_retnclose(req->prod, &msg);
		goto return_prx_cond;
	}

	if (do_stats) {
		struct stats_admin_rule *stats_admin_rule;

		/* We need to provide stats for this request.
		 * FIXME!!! that one is rather dangerous, we want to
		 * make it follow standard rules (eg: clear req->analysers).
		 */

		/* now check whether we have some admin rules for this request */
		list_for_each_entry(stats_admin_rule, &s->be->uri_auth->admin_rules, list) {
			int ret = 1;

			if (stats_admin_rule->cond) {
				ret = acl_exec_cond(stats_admin_rule->cond, s->be, s, &s->txn, ACL_DIR_REQ);
				ret = acl_pass(ret);
				if (stats_admin_rule->cond->pol == ACL_COND_UNLESS)
					ret = !ret;
			}

			if (ret) {
				/* no rule, or the rule matches */
				s->data_ctx.stats.flags |= STAT_ADMIN;
				break;
			}
		}

		/* Was the status page requested with a POST ? */
		if (txn->meth == HTTP_METH_POST) {
			if (s->data_ctx.stats.flags & STAT_ADMIN) {
				if (msg->msg_state < HTTP_MSG_100_SENT) {
					/* If we have HTTP/1.1 and Expect: 100-continue, then we must
					 * send an HTTP/1.1 100 Continue intermediate response.
					 */
					if (txn->flags & TX_REQ_VER_11) {
						struct hdr_ctx ctx;
						ctx.idx = 0;
						/* Expect is allowed in 1.1, look for it */
						if (http_find_header2("Expect", 6, msg->sol, &txn->hdr_idx, &ctx) &&
						    unlikely(ctx.vlen == 12 && strncasecmp(ctx.line+ctx.val, "100-continue", 12) == 0)) {
							buffer_write(s->rep, http_100_chunk.str, http_100_chunk.len);
						}
					}
					msg->msg_state = HTTP_MSG_100_SENT;
					s->logs.tv_request = now;  /* update the request timer to reflect full request */
				}
				if (!http_process_req_stat_post(s, req)) {
					/* we need more data */
					req->analysers |= an_bit;
					buffer_dont_connect(req);
					return 0;
				}
			} else {
				s->data_ctx.stats.st_code = STAT_STATUS_DENY;
			}
		}

		s->logs.tv_request = now;
		s->data_source = DATA_SRC_STATS;
		s->data_state  = DATA_ST_INIT;
		s->task->nice = -32; /* small boost for HTTP statistics */
		stream_int_register_handler(s->rep->prod, http_stats_io_handler);
		s->rep->prod->private = s;
		s->rep->prod->st0 = s->rep->prod->st1 = 0;
		req->analysers = 0;

		return 0;

	}

	/* check whether we have some ACLs set to redirect this request */
	list_for_each_entry(rule, &px->redirect_rules, list) {
		int ret = ACL_PAT_PASS;

		if (rule->cond) {
			ret = acl_exec_cond(rule->cond, px, s, txn, ACL_DIR_REQ);
			ret = acl_pass(ret);
			if (rule->cond->pol == ACL_COND_UNLESS)
				ret = !ret;
		}

		if (ret) {
			struct chunk rdr = { .str = trash, .size = trashlen, .len = 0 };
			const char *msg_fmt;

			/* build redirect message */
			switch(rule->code) {
			case 308:
				msg_fmt = HTTP_308;
				break;
			case 307:
				msg_fmt = HTTP_307;
				break;
			case 303:
				msg_fmt = HTTP_303;
				break;
			case 301:
				msg_fmt = HTTP_301;
				break;
			case 302:
			default:
				msg_fmt = HTTP_302;
				break;
			}

			if (unlikely(!chunk_strcpy(&rdr, msg_fmt)))
				goto return_bad_req;

			switch(rule->type) {
			case REDIRECT_TYPE_PREFIX: {
				const char *path;
				int pathlen;

				path = http_get_path(txn);
				/* build message using path */
				if (path) {
					pathlen = txn->req.sl.rq.u_l + (txn->req.sol + txn->req.sl.rq.u) - path;
					if (rule->flags & REDIRECT_FLAG_DROP_QS) {
						int qs = 0;
						while (qs < pathlen) {
							if (path[qs] == '?') {
								pathlen = qs;
								break;
							}
							qs++;
						}
					}
				} else {
					path = "/";
					pathlen = 1;
				}

				if (rdr.len + rule->rdr_len + pathlen > rdr.size - 4)
					goto return_bad_req;

				/* add prefix. Note that if prefix == "/", we don't want to
				 * add anything, otherwise it makes it hard for the user to
				 * configure a self-redirection.
				 */
				if (rule->rdr_len != 1 || *rule->rdr_str != '/') {
					memcpy(rdr.str + rdr.len, rule->rdr_str, rule->rdr_len);
					rdr.len += rule->rdr_len;
				}

				/* add path */
				memcpy(rdr.str + rdr.len, path, pathlen);
				rdr.len += pathlen;

				/* append a slash at the end of the location is needed and missing */
				if (rdr.len && rdr.str[rdr.len - 1] != '/' &&
				    (rule->flags & REDIRECT_FLAG_APPEND_SLASH)) {
					if (rdr.len > rdr.size - 5)
						goto return_bad_req;
					rdr.str[rdr.len] = '/';
					rdr.len++;
				}

				break;
			}
			case REDIRECT_TYPE_LOCATION:
			default:
				if (rdr.len + rule->rdr_len > rdr.size - 4)
					goto return_bad_req;

				/* add location */
				memcpy(rdr.str + rdr.len, rule->rdr_str, rule->rdr_len);
				rdr.len += rule->rdr_len;
				break;
			}

			if (rule->cookie_len) {
				memcpy(rdr.str + rdr.len, "\r\nSet-Cookie: ", 14);
				rdr.len += 14;
				memcpy(rdr.str + rdr.len, rule->cookie_str, rule->cookie_len);
				rdr.len += rule->cookie_len;
				memcpy(rdr.str + rdr.len, "\r\n", 2);
				rdr.len += 2;
			}

			/* add end of headers and the keep-alive/close status.
			 * We may choose to set keep-alive if the Location begins
			 * with a slash, because the client will come back to the
			 * same server.
			 */
			txn->status = rule->code;
			/* let's log the request time */
			s->logs.tv_request = now;

			if (rule->rdr_len >= 1 && *rule->rdr_str == '/' &&
			    (txn->flags & TX_REQ_XFER_LEN) &&
			    !(txn->flags & TX_REQ_TE_CHNK) && !txn->req.body_len &&
			    ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL ||
			     (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL)) {
				/* keep-alive possible */
				if (!(txn->flags & TX_REQ_VER_11)) {
					if (unlikely(txn->flags & TX_USE_PX_CONN)) {
						memcpy(rdr.str + rdr.len, "\r\nProxy-Connection: keep-alive", 30);
						rdr.len += 30;
					} else {
						memcpy(rdr.str + rdr.len, "\r\nConnection: keep-alive", 24);
						rdr.len += 24;
					}
				}
				memcpy(rdr.str + rdr.len, "\r\n\r\n", 4);
				rdr.len += 4;
				buffer_write(req->prod->ob, rdr.str, rdr.len);
				/* "eat" the request */
				buffer_ignore(req, msg->sov - msg->som);
				msg->som = msg->sov;
				req->analysers = AN_REQ_HTTP_XFER_BODY;
				s->rep->analysers = AN_RES_HTTP_XFER_BODY;
				txn->req.msg_state = HTTP_MSG_CLOSED;
				txn->rsp.msg_state = HTTP_MSG_DONE;
				if (!(s->flags & SN_ERR_MASK))
					s->flags |= SN_ERR_PRXCOND;
				if (!(s->flags & SN_FINST_MASK))
					s->flags |= SN_FINST_R;
				break;
			} else {
				/* keep-alive not possible */
				if (unlikely(txn->flags & TX_USE_PX_CONN)) {
					memcpy(rdr.str + rdr.len, "\r\nProxy-Connection: close\r\n\r\n", 29);
					rdr.len += 29;
				} else {
					memcpy(rdr.str + rdr.len, "\r\nConnection: close\r\n\r\n", 23);
					rdr.len += 23;
				}
				stream_int_retnclose(req->prod, &rdr);
				goto return_prx_cond;
			}
		}
	}

	/* POST requests may be accompanied with an "Expect: 100-Continue" header.
	 * If this happens, then the data will not come immediately, so we must
	 * send all what we have without waiting. Note that due to the small gain
	 * in waiting for the body of the request, it's easier to simply put the
	 * BF_SEND_DONTWAIT flag any time. It's a one-shot flag so it will remove
	 * itself once used.
	 */
	req->flags |= BF_SEND_DONTWAIT;

	/* that's OK for us now, let's move on to next analysers */
	return 1;

 return_bad_req:
	/* We centralize bad requests processing here */
	if (unlikely(msg->msg_state == HTTP_MSG_ERROR) || msg->err_pos >= 0) {
		/* we detected a parsing error. We want to archive this request
		 * in the dedicated proxy area for later troubleshooting.
		 */
		http_capture_bad_message(&s->fe->invalid_req, s, req, msg, msg->msg_state, s->fe);
	}

	txn->req.msg_state = HTTP_MSG_ERROR;
	txn->status = 400;
	stream_int_retnclose(req->prod, error_message(s, HTTP_ERR_400));

	s->fe->counters.failed_req++;
	if (s->listener->counters)
		s->listener->counters->failed_req++;

 return_prx_cond:
	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK))
		s->flags |= SN_FINST_R;

	req->analysers = 0;
	req->analyse_exp = TICK_ETERNITY;
	return 0;
}