static void queue_request(struct intel_engine_cs *engine,
			  struct i915_sched_node *node,
			  int prio)
{
	GEM_BUG_ON(!list_empty(&node->link));
	list_add_tail(&node->link, i915_sched_lookup_priolist(engine, prio));
}