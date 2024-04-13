void intel_execlists_show_requests(struct intel_engine_cs *engine,
				   struct drm_printer *m,
				   void (*show_request)(struct drm_printer *m,
							struct i915_request *rq,
							const char *prefix),
				   unsigned int max)
{
	const struct intel_engine_execlists *execlists = &engine->execlists;
	struct i915_request *rq, *last;
	unsigned long flags;
	unsigned int count;
	struct rb_node *rb;

	spin_lock_irqsave(&engine->active.lock, flags);

	last = NULL;
	count = 0;
	list_for_each_entry(rq, &engine->active.requests, sched.link) {
		if (count++ < max - 1)
			show_request(m, rq, "\t\tE ");
		else
			last = rq;
	}
	if (last) {
		if (count > max) {
			drm_printf(m,
				   "\t\t...skipping %d executing requests...\n",
				   count - max);
		}
		show_request(m, last, "\t\tE ");
	}

	last = NULL;
	count = 0;
	if (execlists->queue_priority_hint != INT_MIN)
		drm_printf(m, "\t\tQueue priority hint: %d\n",
			   execlists->queue_priority_hint);
	for (rb = rb_first_cached(&execlists->queue); rb; rb = rb_next(rb)) {
		struct i915_priolist *p = rb_entry(rb, typeof(*p), node);
		int i;

		priolist_for_each_request(rq, p, i) {
			if (count++ < max - 1)
				show_request(m, rq, "\t\tQ ");
			else
				last = rq;
		}
	}
	if (last) {
		if (count > max) {
			drm_printf(m,
				   "\t\t...skipping %d queued requests...\n",
				   count - max);
		}
		show_request(m, last, "\t\tQ ");
	}

	last = NULL;
	count = 0;
	for (rb = rb_first_cached(&execlists->virtual); rb; rb = rb_next(rb)) {
		struct virtual_engine *ve =
			rb_entry(rb, typeof(*ve), nodes[engine->id].rb);
		struct i915_request *rq = READ_ONCE(ve->request);

		if (rq) {
			if (count++ < max - 1)
				show_request(m, rq, "\t\tV ");
			else
				last = rq;
		}
	}
	if (last) {
		if (count > max) {
			drm_printf(m,
				   "\t\t...skipping %d virtual requests...\n",
				   count - max);
		}
		show_request(m, last, "\t\tV ");
	}

	spin_unlock_irqrestore(&engine->active.lock, flags);
}