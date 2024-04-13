assert_pending_valid(const struct intel_engine_execlists *execlists,
		     const char *msg)
{
	struct i915_request * const *port, *rq;
	struct intel_context *ce = NULL;

	trace_ports(execlists, msg, execlists->pending);

	if (!execlists->pending[0]) {
		GEM_TRACE_ERR("Nothing pending for promotion!\n");
		return false;
	}

	if (execlists->pending[execlists_num_ports(execlists)]) {
		GEM_TRACE_ERR("Excess pending[%d] for promotion!\n",
			      execlists_num_ports(execlists));
		return false;
	}

	for (port = execlists->pending; (rq = *port); port++) {
		if (ce == rq->hw_context) {
			GEM_TRACE_ERR("Duplicate context in pending[%zd]\n",
				      port - execlists->pending);
			return false;
		}

		ce = rq->hw_context;
		if (i915_request_completed(rq))
			continue;

		if (i915_active_is_idle(&ce->active)) {
			GEM_TRACE_ERR("Inactive context in pending[%zd]\n",
				      port - execlists->pending);
			return false;
		}

		if (!i915_vma_is_pinned(ce->state)) {
			GEM_TRACE_ERR("Unpinned context in pending[%zd]\n",
				      port - execlists->pending);
			return false;
		}

		if (!i915_vma_is_pinned(ce->ring->vma)) {
			GEM_TRACE_ERR("Unpinned ringbuffer in pending[%zd]\n",
				      port - execlists->pending);
			return false;
		}
	}

	return ce;
}