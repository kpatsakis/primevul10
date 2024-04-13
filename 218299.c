__execlists_context_pin(struct intel_context *ce,
			struct intel_engine_cs *engine)
{
	void *vaddr;
	int ret;

	GEM_BUG_ON(!ce->state);

	ret = intel_context_active_acquire(ce);
	if (ret)
		goto err;
	GEM_BUG_ON(!i915_vma_is_pinned(ce->state));

	vaddr = i915_gem_object_pin_map(ce->state->obj,
					i915_coherent_map_type(engine->i915) |
					I915_MAP_OVERRIDE);
	if (IS_ERR(vaddr)) {
		ret = PTR_ERR(vaddr);
		goto unpin_active;
	}

	ce->lrc_desc = lrc_descriptor(ce, engine);
	ce->lrc_reg_state = vaddr + LRC_STATE_PN * PAGE_SIZE;
	__execlists_update_reg_state(ce, engine);

	return 0;

unpin_active:
	intel_context_active_release(ce);
err:
	return ret;
}