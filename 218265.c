static int queue_prio(const struct intel_engine_execlists *execlists)
{
	struct i915_priolist *p;
	struct rb_node *rb;

	rb = rb_first_cached(&execlists->queue);
	if (!rb)
		return INT_MIN;

	/*
	 * As the priolist[] are inverted, with the highest priority in [0],
	 * we have to flip the index value to become priority.
	 */
	p = to_priolist(rb);
	return ((p->priority + 1) << I915_USER_PRIORITY_SHIFT) - ffs(p->used);
}