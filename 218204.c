static bool unexpected_starting_state(struct intel_engine_cs *engine)
{
	bool unexpected = false;

	if (ENGINE_READ_FW(engine, RING_MI_MODE) & STOP_RING) {
		DRM_DEBUG_DRIVER("STOP_RING still set in RING_MI_MODE\n");
		unexpected = true;
	}

	return unexpected;
}