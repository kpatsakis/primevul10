static void __execlists_context_fini(struct intel_context *ce)
{
	intel_ring_put(ce->ring);
	i915_vma_put(ce->state);
}