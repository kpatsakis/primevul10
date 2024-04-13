populate_lr_context(struct intel_context *ce,
		    struct drm_i915_gem_object *ctx_obj,
		    struct intel_engine_cs *engine,
		    struct intel_ring *ring)
{
	bool inhibit = true;
	void *vaddr;
	u32 *regs;
	int ret;

	vaddr = i915_gem_object_pin_map(ctx_obj, I915_MAP_WB);
	if (IS_ERR(vaddr)) {
		ret = PTR_ERR(vaddr);
		DRM_DEBUG_DRIVER("Could not map object pages! (%d)\n", ret);
		return ret;
	}

	set_redzone(vaddr, engine);

	if (engine->default_state) {
		void *defaults;

		defaults = i915_gem_object_pin_map(engine->default_state,
						   I915_MAP_WB);
		if (IS_ERR(defaults)) {
			ret = PTR_ERR(defaults);
			goto err_unpin_ctx;
		}

		memcpy(vaddr, defaults, engine->context_size);
		i915_gem_object_unpin_map(engine->default_state);
		inhibit = false;
	}

	/* The second page of the context object contains some fields which must
	 * be set up prior to the first execution. */
	regs = vaddr + LRC_STATE_PN * PAGE_SIZE;
	execlists_init_reg_state(regs, ce, engine, ring, inhibit);
	if (inhibit)
		regs[CTX_CONTEXT_CONTROL] |=
			_MASKED_BIT_ENABLE(CTX_CTRL_ENGINE_CTX_RESTORE_INHIBIT);

	ret = 0;
err_unpin_ctx:
	__i915_gem_object_flush_map(ctx_obj, 0, engine->context_size);
	i915_gem_object_unpin_map(ctx_obj);
	return ret;
}