static void execlists_context_destroy(struct kref *kref)
{
	struct intel_context *ce = container_of(kref, typeof(*ce), ref);

	GEM_BUG_ON(!i915_active_is_idle(&ce->active));
	GEM_BUG_ON(intel_context_is_pinned(ce));

	if (ce->state)
		__execlists_context_fini(ce);

	intel_context_fini(ce);
	intel_context_free(ce);
}