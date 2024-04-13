static void tctx_task_work(struct callback_head *cb)
{
	struct io_ring_ctx *ctx = NULL;
	struct io_uring_task *tctx = container_of(cb, struct io_uring_task,
						  task_work);

	while (1) {
		struct io_wq_work_node *node;

		spin_lock_irq(&tctx->task_lock);
		node = tctx->task_list.first;
		INIT_WQ_LIST(&tctx->task_list);
		spin_unlock_irq(&tctx->task_lock);

		while (node) {
			struct io_wq_work_node *next = node->next;
			struct io_kiocb *req = container_of(node, struct io_kiocb,
							    io_task_work.node);

			if (req->ctx != ctx) {
				ctx_flush_and_put(ctx);
				ctx = req->ctx;
				percpu_ref_get(&ctx->refs);
			}
			req->io_task_work.func(req);
			node = next;
		}
		if (wq_list_empty(&tctx->task_list)) {
			clear_bit(0, &tctx->task_state);
			if (wq_list_empty(&tctx->task_list))
				break;
			/* another tctx_task_work() is enqueued, yield */
			if (test_and_set_bit(0, &tctx->task_state))
				break;
		}
		cond_resched();
	}

	ctx_flush_and_put(ctx);
}