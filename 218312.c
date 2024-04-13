static void virtual_context_enter(struct intel_context *ce)
{
	struct virtual_engine *ve = container_of(ce, typeof(*ve), context);
	unsigned int n;

	for (n = 0; n < ve->num_siblings; n++)
		intel_engine_pm_get(ve->siblings[n]);

	intel_timeline_enter(ce->timeline);
}