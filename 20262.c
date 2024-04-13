
void __io_uring_free(struct task_struct *tsk)
{
	struct io_uring_task *tctx = tsk->io_uring;

	WARN_ON_ONCE(!xa_empty(&tctx->xa));
	WARN_ON_ONCE(tctx->io_wq);
	WARN_ON_ONCE(tctx->cached_refs);

	percpu_counter_destroy(&tctx->inflight);
	kfree(tctx);
	tsk->io_uring = NULL;