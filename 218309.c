static void execlists_destroy(struct intel_engine_cs *engine)
{
	intel_engine_cleanup_common(engine);
	lrc_destroy_wa_ctx(engine);
	kfree(engine);
}