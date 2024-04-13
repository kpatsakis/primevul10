check_redzone(const void *vaddr, const struct intel_engine_cs *engine)
{
	if (!IS_ENABLED(CONFIG_DRM_I915_DEBUG_GEM))
		return;

	vaddr += engine->context_size;

	if (memchr_inv(vaddr, POISON_INUSE, I915_GTT_PAGE_SIZE))
		dev_err_once(engine->i915->drm.dev,
			     "%s context redzone overwritten!\n",
			     engine->name);
}