void http_reset_txn(struct session *s)
{
	http_end_txn(s);
	http_init_txn(s);

	s->be = s->fe;
	s->req->analysers = s->listener->analysers;
	s->logs.logwait = s->fe->to_log;
	s->srv = s->prev_srv = s->srv_conn = NULL;
	/* re-init store persistence */
	s->store_count = 0;

	s->pend_pos = NULL;
	s->conn_retries = s->be->conn_retries;

	s->req->flags |= BF_READ_DONTWAIT; /* one read is usually enough */

	/* We must trim any excess data from the response buffer, because we
	 * may have blocked an invalid response from a server that we don't
	 * want to accidentely forward once we disable the analysers, nor do
	 * we want those data to come along with next response. A typical
	 * example of such data would be from a buggy server responding to
	 * a HEAD with some data, or sending more than the advertised
	 * content-length.
	 */
	if (unlikely(s->rep->l > s->rep->send_max)) {
		s->rep->l = s->rep->send_max;
		s->rep->r = s->rep->w + s->rep->l;
		if (s->rep->r >= s->rep->data + s->rep->size)
			s->rep->r -= s->rep->size;
	}

	s->req->rto = s->fe->timeout.client;
	s->req->wto = s->be->timeout.server;
	s->req->cto = s->be->timeout.connect;

	s->rep->rto = s->be->timeout.server;
	s->rep->wto = s->fe->timeout.client;
	s->rep->cto = TICK_ETERNITY;

	s->req->rex = TICK_ETERNITY;
	s->req->wex = TICK_ETERNITY;
	s->req->analyse_exp = TICK_ETERNITY;
	s->rep->rex = TICK_ETERNITY;
	s->rep->wex = TICK_ETERNITY;
	s->rep->analyse_exp = TICK_ETERNITY;
}