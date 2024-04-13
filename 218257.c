assert_priority_queue(const struct i915_request *prev,
		      const struct i915_request *next)
{
	/*
	 * Without preemption, the prev may refer to the still active element
	 * which we refuse to let go.
	 *
	 * Even with preemption, there are times when we think it is better not
	 * to preempt and leave an ostensibly lower priority request in flight.
	 */
	if (i915_request_is_active(prev))
		return true;

	return rq_prio(prev) >= rq_prio(next);
}