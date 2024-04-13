static void execlists_submit_request(struct i915_request *request)
{
	struct intel_engine_cs *engine = request->engine;
	unsigned long flags;

	/* Will be called from irq-context when using foreign fences. */
	spin_lock_irqsave(&engine->active.lock, flags);

	queue_request(engine, &request->sched, rq_prio(request));

	GEM_BUG_ON(RB_EMPTY_ROOT(&engine->execlists.queue.rb_root));
	GEM_BUG_ON(list_empty(&request->sched.link));

	submit_queue(engine, request);

	spin_unlock_irqrestore(&engine->active.lock, flags);
}