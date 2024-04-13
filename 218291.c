static void kick_siblings(struct i915_request *rq, struct intel_context *ce)
{
	struct virtual_engine *ve = container_of(ce, typeof(*ve), context);
	struct i915_request *next = READ_ONCE(ve->request);

	if (next && next->execution_mask & ~rq->execution_mask)
		tasklet_schedule(&ve->base.execlists.tasklet);
}