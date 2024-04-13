active_request(const struct intel_timeline * const tl, struct i915_request *rq)
{
	struct i915_request *active = rq;

	rcu_read_lock();
	list_for_each_entry_continue_reverse(rq, &tl->requests, link) {
		if (i915_request_completed(rq))
			break;

		active = rq;
	}
	rcu_read_unlock();

	return active;
}