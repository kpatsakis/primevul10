
static int io_uring_alloc_task_context(struct task_struct *task,
				       struct io_ring_ctx *ctx)
{
	struct io_uring_task *tctx;
	int ret;

	tctx = kzalloc(sizeof(*tctx), GFP_KERNEL);
	if (unlikely(!tctx))
		return -ENOMEM;

	ret = percpu_counter_init(&tctx->inflight, 0, GFP_KERNEL);
	if (unlikely(ret)) {
		kfree(tctx);
		return ret;
	}

	tctx->io_wq = io_init_wq_offload(ctx, task);
	if (IS_ERR(tctx->io_wq)) {
		ret = PTR_ERR(tctx->io_wq);
		percpu_counter_destroy(&tctx->inflight);
		kfree(tctx);
		return ret;
	}

	xa_init(&tctx->xa);
	init_waitqueue_head(&tctx->wait);
	atomic_set(&tctx->in_idle, 0);
	atomic_set(&tctx->inflight_tracked, 0);
	task->io_uring = tctx;
	spin_lock_init(&tctx->task_lock);
	INIT_WQ_LIST(&tctx->task_list);
	init_task_work(&tctx->task_work, tctx_task_work);
	return 0;