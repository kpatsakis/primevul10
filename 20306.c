static void io_fallback_req_func(struct work_struct *work)
{
	struct io_ring_ctx *ctx = container_of(work, struct io_ring_ctx,
						fallback_work.work);
	struct llist_node *node = llist_del_all(&ctx->fallback_llist);
	struct io_kiocb *req, *tmp;

	llist_for_each_entry_safe(req, tmp, node, io_task_work.fallback_node)
		req->io_task_work.func(req);
}