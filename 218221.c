static void virtual_context_exit(struct intel_context *ce)
{
	struct virtual_engine *ve = container_of(ce, typeof(*ve), context);
	unsigned int n;

	intel_timeline_exit(ce->timeline);

	for (n = 0; n < ve->num_siblings; n++)
		intel_engine_pm_put(ve->siblings[n]);
}