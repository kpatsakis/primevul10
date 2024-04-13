static int __execlists_context_alloc(struct intel_context *ce,
				     struct intel_engine_cs *engine)
{
	struct drm_i915_gem_object *ctx_obj;
	struct intel_ring *ring;
	struct i915_vma *vma;
	u32 context_size;
	int ret;

	GEM_BUG_ON(ce->state);
	context_size = round_up(engine->context_size, I915_GTT_PAGE_SIZE);

	if (IS_ENABLED(CONFIG_DRM_I915_DEBUG_GEM))
		context_size += I915_GTT_PAGE_SIZE; /* for redzone */

	ctx_obj = i915_gem_object_create_shmem(engine->i915, context_size);
	if (IS_ERR(ctx_obj))
		return PTR_ERR(ctx_obj);

	vma = i915_vma_instance(ctx_obj, &engine->gt->ggtt->vm, NULL);
	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto error_deref_obj;
	}

	if (!ce->timeline) {
		struct intel_timeline *tl;

		tl = intel_timeline_create(engine->gt, NULL);
		if (IS_ERR(tl)) {
			ret = PTR_ERR(tl);
			goto error_deref_obj;
		}

		ce->timeline = tl;
	}

	ring = intel_engine_create_ring(engine, (unsigned long)ce->ring);
	if (IS_ERR(ring)) {
		ret = PTR_ERR(ring);
		goto error_deref_obj;
	}

	ret = populate_lr_context(ce, ctx_obj, engine, ring);
	if (ret) {
		DRM_DEBUG_DRIVER("Failed to populate LRC: %d\n", ret);
		goto error_ring_free;
	}

	ce->ring = ring;
	ce->state = vma;

	return 0;

error_ring_free:
	intel_ring_put(ring);
error_deref_obj:
	i915_gem_object_put(ctx_obj);
	return ret;
}