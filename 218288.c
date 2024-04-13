static intel_engine_mask_t virtual_submission_mask(struct virtual_engine *ve)
{
	struct i915_request *rq;
	intel_engine_mask_t mask;

	rq = READ_ONCE(ve->request);
	if (!rq)
		return 0;

	/* The rq is ready for submission; rq->execution_mask is now stable. */
	mask = rq->execution_mask;
	if (unlikely(!mask)) {
		/* Invalid selection, submit to a random engine in error */
		i915_request_skip(rq, -ENODEV);
		mask = ve->siblings[0]->mask;
	}

	GEM_TRACE("%s: rq=%llx:%lld, mask=%x, prio=%d\n",
		  ve->base.name,
		  rq->fence.context, rq->fence.seqno,
		  mask, ve->base.execlists.queue_priority_hint);

	return mask;
}