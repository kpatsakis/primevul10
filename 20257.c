
static void io_uring_clean_tctx(struct io_uring_task *tctx)
{
	struct io_wq *wq = tctx->io_wq;
	struct io_tctx_node *node;
	unsigned long index;

	xa_for_each(&tctx->xa, index, node)
		io_uring_del_tctx_node(index);
	if (wq) {
		/*
		 * Must be after io_uring_del_task_file() (removes nodes under
		 * uring_lock) to avoid race with io_uring_try_cancel_iowq().
		 */
		tctx->io_wq = NULL;
		io_wq_put_and_exit(wq);
	}