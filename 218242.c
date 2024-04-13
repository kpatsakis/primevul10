last_active(const struct intel_engine_execlists *execlists)
{
	struct i915_request * const *last = READ_ONCE(execlists->active);

	while (*last && i915_request_completed(*last))
		last++;

	return *last;
}