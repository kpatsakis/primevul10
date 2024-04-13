static int lrc_ring_mi_mode(const struct intel_engine_cs *engine)
{
	if (INTEL_GEN(engine->i915) >= 12)
		return 0x60;
	else if (INTEL_GEN(engine->i915) >= 9)
		return 0x54;
	else if (engine->class == RENDER_CLASS)
		return 0x58;
	else
		return -1;
}