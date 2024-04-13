cancel_port_requests(struct intel_engine_execlists * const execlists)
{
	struct i915_request * const *port;

	for (port = execlists->pending; *port; port++)
		execlists_schedule_out(*port);
	memset(execlists->pending, 0, sizeof(execlists->pending));

	/* Mark the end of active before we overwrite *active */
	for (port = xchg(&execlists->active, execlists->pending); *port; port++)
		execlists_schedule_out(*port);
	WRITE_ONCE(execlists->active,
		   memset(execlists->inflight, 0, sizeof(execlists->inflight)));
}