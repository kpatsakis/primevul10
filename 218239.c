static void execlists_reset(struct intel_engine_cs *engine, bool stalled)
{
	unsigned long flags;

	GEM_TRACE("%s\n", engine->name);

	spin_lock_irqsave(&engine->active.lock, flags);

	__execlists_reset(engine, stalled);

	spin_unlock_irqrestore(&engine->active.lock, flags);
}