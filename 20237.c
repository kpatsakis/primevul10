static void io_iopoll_req_issued(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;
	const bool in_async = io_wq_current_is_worker();

	/* workqueue context doesn't hold uring_lock, grab it now */
	if (unlikely(in_async))
		mutex_lock(&ctx->uring_lock);

	/*
	 * Track whether we have multiple files in our lists. This will impact
	 * how we do polling eventually, not spinning if we're on potentially
	 * different devices.
	 */
	if (list_empty(&ctx->iopoll_list)) {
		ctx->poll_multi_queue = false;
	} else if (!ctx->poll_multi_queue) {
		struct io_kiocb *list_req;
		unsigned int queue_num0, queue_num1;

		list_req = list_first_entry(&ctx->iopoll_list, struct io_kiocb,
						inflight_entry);

		if (list_req->file != req->file) {
			ctx->poll_multi_queue = true;
		} else {
			queue_num0 = blk_qc_t_to_queue_num(list_req->rw.kiocb.ki_cookie);
			queue_num1 = blk_qc_t_to_queue_num(req->rw.kiocb.ki_cookie);
			if (queue_num0 != queue_num1)
				ctx->poll_multi_queue = true;
		}
	}

	/*
	 * For fast devices, IO may have already completed. If it has, add
	 * it to the front so we find it first.
	 */
	if (READ_ONCE(req->iopoll_completed))
		list_add(&req->inflight_entry, &ctx->iopoll_list);
	else
		list_add_tail(&req->inflight_entry, &ctx->iopoll_list);

	if (unlikely(in_async)) {
		/*
		 * If IORING_SETUP_SQPOLL is enabled, sqes are either handle
		 * in sq thread task context or in io worker task context. If
		 * current task context is sq thread, we don't need to check
		 * whether should wake up sq thread.
		 */
		if ((ctx->flags & IORING_SETUP_SQPOLL) &&
		    wq_has_sleeper(&ctx->sq_data->wait))
			wake_up(&ctx->sq_data->wait);

		mutex_unlock(&ctx->uring_lock);
	}
}