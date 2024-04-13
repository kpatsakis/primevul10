static void virtual_xfer_breadcrumbs(struct virtual_engine *ve,
				     struct intel_engine_cs *engine)
{
	struct intel_engine_cs *old = ve->siblings[0];

	/* All unattached (rq->engine == old) must already be completed */

	spin_lock(&old->breadcrumbs.irq_lock);
	if (!list_empty(&ve->context.signal_link)) {
		list_move_tail(&ve->context.signal_link,
			       &engine->breadcrumbs.signalers);
		intel_engine_queue_breadcrumbs(engine);
	}
	spin_unlock(&old->breadcrumbs.irq_lock);
}