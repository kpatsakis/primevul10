
static bool io_cancel_task_cb(struct io_wq_work *work, void *data)
{
	struct io_kiocb *req = container_of(work, struct io_kiocb, work);
	struct io_task_cancel *cancel = data;
	bool ret;

	if (!cancel->all && (req->flags & REQ_F_LINK_TIMEOUT)) {
		unsigned long flags;
		struct io_ring_ctx *ctx = req->ctx;

		/* protect against races with linked timeouts */
		spin_lock_irqsave(&ctx->completion_lock, flags);
		ret = io_match_task(req, cancel->task, cancel->all);
		spin_unlock_irqrestore(&ctx->completion_lock, flags);
	} else {
		ret = io_match_task(req, cancel->task, cancel->all);
	}
	return ret;