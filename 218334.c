ring_set_paused(const struct intel_engine_cs *engine, int state)
{
	/*
	 * We inspect HWS_PREEMPT with a semaphore inside
	 * engine->emit_fini_breadcrumb. If the dword is true,
	 * the ring is paused as the semaphore will busywait
	 * until the dword is false.
	 */
	engine->status_page.addr[I915_GEM_HWS_PREEMPT] = state;
	if (state)
		wmb();
}