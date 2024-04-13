 */
static bool io_poll_remove_all(struct io_ring_ctx *ctx, struct task_struct *tsk,
			       bool cancel_all)
{
	struct hlist_node *tmp;
	struct io_kiocb *req;
	int posted = 0, i;

	spin_lock_irq(&ctx->completion_lock);
	for (i = 0; i < (1U << ctx->cancel_hash_bits); i++) {
		struct hlist_head *list;

		list = &ctx->cancel_hash[i];
		hlist_for_each_entry_safe(req, tmp, list, hash_node) {
			if (io_match_task(req, tsk, cancel_all))
				posted += io_poll_remove_one(req);
		}
	}
	spin_unlock_irq(&ctx->completion_lock);

	if (posted)
		io_cqring_ev_posted(ctx);

	return posted != 0;