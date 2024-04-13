static inline int rq_prio(const struct i915_request *rq)
{
	return rq->sched.attr.priority;
}