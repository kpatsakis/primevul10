int intel_execlists_submission_init(struct intel_engine_cs *engine)
{
	struct intel_engine_execlists * const execlists = &engine->execlists;
	struct drm_i915_private *i915 = engine->i915;
	struct intel_uncore *uncore = engine->uncore;
	u32 base = engine->mmio_base;
	int ret;

	ret = intel_engine_init_common(engine);
	if (ret)
		return ret;

	if (intel_init_workaround_bb(engine))
		/*
		 * We continue even if we fail to initialize WA batch
		 * because we only expect rare glitches but nothing
		 * critical to prevent us from using GPU
		 */
		DRM_ERROR("WA batch buffer initialization failed\n");

	if (HAS_LOGICAL_RING_ELSQ(i915)) {
		execlists->submit_reg = uncore->regs +
			i915_mmio_reg_offset(RING_EXECLIST_SQ_CONTENTS(base));
		execlists->ctrl_reg = uncore->regs +
			i915_mmio_reg_offset(RING_EXECLIST_CONTROL(base));
	} else {
		execlists->submit_reg = uncore->regs +
			i915_mmio_reg_offset(RING_ELSP(base));
	}

	execlists->csb_status =
		&engine->status_page.addr[I915_HWS_CSB_BUF0_INDEX];

	execlists->csb_write =
		&engine->status_page.addr[intel_hws_csb_write_index(i915)];

	if (INTEL_GEN(i915) < 11)
		execlists->csb_size = GEN8_CSB_ENTRIES;
	else
		execlists->csb_size = GEN11_CSB_ENTRIES;

	reset_csb_pointers(engine);

	return 0;
}