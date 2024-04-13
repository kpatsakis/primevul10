static void io_req_task_cancel(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;

	/* ctx is guaranteed to stay alive while we hold uring_lock */
	mutex_lock(&ctx->uring_lock);
	io_req_complete_failed(req, req->result);
	mutex_unlock(&ctx->uring_lock);
}