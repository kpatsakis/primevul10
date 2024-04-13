static int virtual_context_pin(struct intel_context *ce)
{
	struct virtual_engine *ve = container_of(ce, typeof(*ve), context);
	int err;

	/* Note: we must use a real engine class for setting up reg state */
	err = __execlists_context_pin(ce, ve->siblings[0]);
	if (err)
		return err;

	virtual_engine_initial_hint(ve);
	return 0;
}