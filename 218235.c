static void virtual_context_destroy(struct kref *kref)
{
	struct virtual_engine *ve =
		container_of(kref, typeof(*ve), context.ref);
	unsigned int n;

	GEM_BUG_ON(!list_empty(virtual_queue(ve)));
	GEM_BUG_ON(ve->request);
	GEM_BUG_ON(ve->context.inflight);

	for (n = 0; n < ve->num_siblings; n++) {
		struct intel_engine_cs *sibling = ve->siblings[n];
		struct rb_node *node = &ve->nodes[sibling->id].rb;
		unsigned long flags;

		if (RB_EMPTY_NODE(node))
			continue;

		spin_lock_irqsave(&sibling->active.lock, flags);

		/* Detachment is lazily performed in the execlists tasklet */
		if (!RB_EMPTY_NODE(node))
			rb_erase_cached(node, &sibling->execlists.virtual);

		spin_unlock_irqrestore(&sibling->active.lock, flags);
	}
	GEM_BUG_ON(__tasklet_is_scheduled(&ve->base.execlists.tasklet));

	if (ve->context.state)
		__execlists_context_fini(&ve->context);
	intel_context_fini(&ve->context);

	kfree(ve->bonds);
	kfree(ve);
}