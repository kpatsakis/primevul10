intel_execlists_create_virtual(struct i915_gem_context *ctx,
			       struct intel_engine_cs **siblings,
			       unsigned int count)
{
	struct virtual_engine *ve;
	unsigned int n;
	int err;

	if (count == 0)
		return ERR_PTR(-EINVAL);

	if (count == 1)
		return intel_context_create(ctx, siblings[0]);

	ve = kzalloc(struct_size(ve, siblings, count), GFP_KERNEL);
	if (!ve)
		return ERR_PTR(-ENOMEM);

	ve->base.i915 = ctx->i915;
	ve->base.gt = siblings[0]->gt;
	ve->base.uncore = siblings[0]->uncore;
	ve->base.id = -1;
	ve->base.class = OTHER_CLASS;
	ve->base.uabi_class = I915_ENGINE_CLASS_INVALID;
	ve->base.instance = I915_ENGINE_CLASS_INVALID_VIRTUAL;

	/*
	 * The decision on whether to submit a request using semaphores
	 * depends on the saturated state of the engine. We only compute
	 * this during HW submission of the request, and we need for this
	 * state to be globally applied to all requests being submitted
	 * to this engine. Virtual engines encompass more than one physical
	 * engine and so we cannot accurately tell in advance if one of those
	 * engines is already saturated and so cannot afford to use a semaphore
	 * and be pessimized in priority for doing so -- if we are the only
	 * context using semaphores after all other clients have stopped, we
	 * will be starved on the saturated system. Such a global switch for
	 * semaphores is less than ideal, but alas is the current compromise.
	 */
	ve->base.saturated = ALL_ENGINES;

	snprintf(ve->base.name, sizeof(ve->base.name), "virtual");

	intel_engine_init_active(&ve->base, ENGINE_VIRTUAL);
	intel_engine_init_breadcrumbs(&ve->base);

	intel_engine_init_execlists(&ve->base);

	ve->base.cops = &virtual_context_ops;
	ve->base.request_alloc = execlists_request_alloc;

	ve->base.schedule = i915_schedule;
	ve->base.submit_request = virtual_submit_request;
	ve->base.bond_execute = virtual_bond_execute;

	INIT_LIST_HEAD(virtual_queue(ve));
	ve->base.execlists.queue_priority_hint = INT_MIN;
	tasklet_init(&ve->base.execlists.tasklet,
		     virtual_submission_tasklet,
		     (unsigned long)ve);

	intel_context_init(&ve->context, ctx, &ve->base);

	for (n = 0; n < count; n++) {
		struct intel_engine_cs *sibling = siblings[n];

		GEM_BUG_ON(!is_power_of_2(sibling->mask));
		if (sibling->mask & ve->base.mask) {
			DRM_DEBUG("duplicate %s entry in load balancer\n",
				  sibling->name);
			err = -EINVAL;
			goto err_put;
		}

		/*
		 * The virtual engine implementation is tightly coupled to
		 * the execlists backend -- we push out request directly
		 * into a tree inside each physical engine. We could support
		 * layering if we handle cloning of the requests and
		 * submitting a copy into each backend.
		 */
		if (sibling->execlists.tasklet.func !=
		    execlists_submission_tasklet) {
			err = -ENODEV;
			goto err_put;
		}

		GEM_BUG_ON(RB_EMPTY_NODE(&ve->nodes[sibling->id].rb));
		RB_CLEAR_NODE(&ve->nodes[sibling->id].rb);

		ve->siblings[ve->num_siblings++] = sibling;
		ve->base.mask |= sibling->mask;

		/*
		 * All physical engines must be compatible for their emission
		 * functions (as we build the instructions during request
		 * construction and do not alter them before submission
		 * on the physical engine). We use the engine class as a guide
		 * here, although that could be refined.
		 */
		if (ve->base.class != OTHER_CLASS) {
			if (ve->base.class != sibling->class) {
				DRM_DEBUG("invalid mixing of engine class, sibling %d, already %d\n",
					  sibling->class, ve->base.class);
				err = -EINVAL;
				goto err_put;
			}
			continue;
		}

		ve->base.class = sibling->class;
		ve->base.uabi_class = sibling->uabi_class;
		snprintf(ve->base.name, sizeof(ve->base.name),
			 "v%dx%d", ve->base.class, count);
		ve->base.context_size = sibling->context_size;

		ve->base.emit_bb_start = sibling->emit_bb_start;
		ve->base.emit_flush = sibling->emit_flush;
		ve->base.emit_init_breadcrumb = sibling->emit_init_breadcrumb;
		ve->base.emit_fini_breadcrumb = sibling->emit_fini_breadcrumb;
		ve->base.emit_fini_breadcrumb_dw =
			sibling->emit_fini_breadcrumb_dw;

		ve->base.flags = sibling->flags;
	}

	ve->base.flags |= I915_ENGINE_IS_VIRTUAL;

	err = __execlists_context_alloc(&ve->context, siblings[0]);
	if (err)
		goto err_put;

	__set_bit(CONTEXT_ALLOC_BIT, &ve->context.flags);

	return &ve->context;

err_put:
	intel_context_put(&ve->context);
	return ERR_PTR(err);
}