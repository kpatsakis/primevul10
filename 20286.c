
static void io_uring_try_cancel_requests(struct io_ring_ctx *ctx,
					 struct task_struct *task,
					 bool cancel_all)
{
	struct io_task_cancel cancel = { .task = task, .all = cancel_all, };
	struct io_uring_task *tctx = task ? task->io_uring : NULL;

	while (1) {
		enum io_wq_cancel cret;
		bool ret = false;

		if (!task) {
			ret |= io_uring_try_cancel_iowq(ctx);
		} else if (tctx && tctx->io_wq) {
			/*
			 * Cancels requests of all rings, not only @ctx, but
			 * it's fine as the task is in exit/exec.
			 */
			cret = io_wq_cancel_cb(tctx->io_wq, io_cancel_task_cb,
					       &cancel, true);
			ret |= (cret != IO_WQ_CANCEL_NOTFOUND);
		}

		/* SQPOLL thread does its own polling */
		if ((!(ctx->flags & IORING_SETUP_SQPOLL) && cancel_all) ||
		    (ctx->sq_data && ctx->sq_data->thread == current)) {
			while (!list_empty_careful(&ctx->iopoll_list)) {
				io_iopoll_try_reap_events(ctx);
				ret = true;
			}
		}

		ret |= io_cancel_defer_files(ctx, task, cancel_all);
		ret |= io_poll_remove_all(ctx, task, cancel_all);
		ret |= io_kill_timeouts(ctx, task, cancel_all);
		if (task)
			ret |= io_run_task_work();
		if (!ret)
			break;
		cond_resched();
	}