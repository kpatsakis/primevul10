static void http_silent_debug(int line, struct session *s)
{
	int size = 0;
	size += snprintf(trash + size, trashlen - size,
			 "[%04d] req: p=%d(%d) s=%d bf=%08x an=%08x data=%p size=%d l=%d w=%p r=%p lr=%p sm=%d fw=%ld tf=%08x\n",
			 line,
			 s->si[0].state, s->si[0].fd, s->txn.req.msg_state, s->req->flags, s->req->analysers,
			 s->req->data, s->req->size, s->req->l, s->req->w, s->req->r, s->req->lr, s->req->send_max, s->req->to_forward, s->txn.flags);
	write(-1, trash, size);
	size = 0;
	size += snprintf(trash + size, trashlen - size,
			 " %04d  rep: p=%d(%d) s=%d bf=%08x an=%08x data=%p size=%d l=%d w=%p r=%p lr=%p sm=%d fw=%ld\n",
			 line,
			 s->si[1].state, s->si[1].fd, s->txn.rsp.msg_state, s->rep->flags, s->rep->analysers,
			 s->rep->data, s->rep->size, s->rep->l, s->rep->w, s->rep->r, s->rep->lr, s->rep->send_max, s->rep->to_forward);

	write(-1, trash, size);
}