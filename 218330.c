static int execlists_context_pin(struct intel_context *ce)
{
	return __execlists_context_pin(ce, ce->engine);
}