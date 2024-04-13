static void io_req_task_work_add(struct io_kiocb *req)
{
	struct task_struct *tsk = req->task;
	struct io_uring_task *tctx = tsk->io_uring;
	enum task_work_notify_mode notify;
	struct io_wq_work_node *node;
	unsigned long flags;

	WARN_ON_ONCE(!tctx);

	spin_lock_irqsave(&tctx->task_lock, flags);
	wq_list_add_tail(&req->io_task_work.node, &tctx->task_list);
	spin_unlock_irqrestore(&tctx->task_lock, flags);

	/* task_work already pending, we're done */
	if (test_bit(0, &tctx->task_state) ||
	    test_and_set_bit(0, &tctx->task_state))
		return;
	if (unlikely(tsk->flags & PF_EXITING))
		goto fail;

	/*
	 * SQPOLL kernel thread doesn't need notification, just a wakeup. For
	 * all other cases, use TWA_SIGNAL unconditionally to ensure we're
	 * processing task_work. There's no reliable way to tell if TWA_RESUME
	 * will do the job.
	 */
	notify = (req->ctx->flags & IORING_SETUP_SQPOLL) ? TWA_NONE : TWA_SIGNAL;
	if (!task_work_add(tsk, &tctx->task_work, notify)) {
		wake_up_process(tsk);
		return;
	}
fail:
	clear_bit(0, &tctx->task_state);
	spin_lock_irqsave(&tctx->task_lock, flags);
	node = tctx->task_list.first;
	INIT_WQ_LIST(&tctx->task_list);
	spin_unlock_irqrestore(&tctx->task_lock, flags);

	while (node) {
		req = container_of(node, struct io_kiocb, io_task_work.node);
		node = node->next;
		if (llist_add(&req->io_task_work.fallback_node,
			      &req->ctx->fallback_llist))
			schedule_delayed_work(&req->ctx->fallback_work, 1);
	}
}