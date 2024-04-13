static int execlists_context_alloc(struct intel_context *ce)
{
	return __execlists_context_alloc(ce, ce->engine);
}