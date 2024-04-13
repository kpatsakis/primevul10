static void execlists_submission_tasklet(unsigned long data)
{
	struct intel_engine_cs * const engine = (struct intel_engine_cs *)data;
	bool timeout = preempt_timeout(engine);

	process_csb(engine);
	if (!READ_ONCE(engine->execlists.pending[0]) || timeout) {
		unsigned long flags;

		spin_lock_irqsave(&engine->active.lock, flags);
		__execlists_submission_tasklet(engine);
		spin_unlock_irqrestore(&engine->active.lock, flags);

		/* Recheck after serialising with direct-submission */
		if (timeout && preempt_timeout(engine))
			preempt_reset(engine);
	}
}