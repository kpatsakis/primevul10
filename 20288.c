
static inline void io_queue_sqe(struct io_kiocb *req)
{
	if (unlikely(req->ctx->drain_active) && io_drain_req(req))
		return;

	if (likely(!(req->flags & REQ_F_FORCE_ASYNC))) {
		__io_queue_sqe(req);
	} else {
		int ret = io_req_prep_async(req);

		if (unlikely(ret))
			io_req_complete_failed(req, ret);
		else
			io_queue_async_work(req);
	}