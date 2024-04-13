trace_ports(const struct intel_engine_execlists *execlists,
	    const char *msg,
	    struct i915_request * const *ports)
{
	const struct intel_engine_cs *engine =
		container_of(execlists, typeof(*engine), execlists);

	if (!ports[0])
		return;

	GEM_TRACE("%s: %s { %llx:%lld%s, %llx:%lld }\n",
		  engine->name, msg,
		  ports[0]->fence.context,
		  ports[0]->fence.seqno,
		  i915_request_completed(ports[0]) ? "!" :
		  i915_request_started(ports[0]) ? "*" :
		  "",
		  ports[1] ? ports[1]->fence.context : 0,
		  ports[1] ? ports[1]->fence.seqno : 0);
}