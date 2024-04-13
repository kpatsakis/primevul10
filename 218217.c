static void execlists_dequeue(struct intel_engine_cs *engine)
{
	struct intel_engine_execlists * const execlists = &engine->execlists;
	struct i915_request **port = execlists->pending;
	struct i915_request ** const last_port = port + execlists->port_mask;
	struct i915_request *last;
	struct rb_node *rb;
	bool submit = false;

	/*
	 * Hardware submission is through 2 ports. Conceptually each port
	 * has a (RING_START, RING_HEAD, RING_TAIL) tuple. RING_START is
	 * static for a context, and unique to each, so we only execute
	 * requests belonging to a single context from each ring. RING_HEAD
	 * is maintained by the CS in the context image, it marks the place
	 * where it got up to last time, and through RING_TAIL we tell the CS
	 * where we want to execute up to this time.
	 *
	 * In this list the requests are in order of execution. Consecutive
	 * requests from the same context are adjacent in the ringbuffer. We
	 * can combine these requests into a single RING_TAIL update:
	 *
	 *              RING_HEAD...req1...req2
	 *                                    ^- RING_TAIL
	 * since to execute req2 the CS must first execute req1.
	 *
	 * Our goal then is to point each port to the end of a consecutive
	 * sequence of requests as being the most optimal (fewest wake ups
	 * and context switches) submission.
	 */

	for (rb = rb_first_cached(&execlists->virtual); rb; ) {
		struct virtual_engine *ve =
			rb_entry(rb, typeof(*ve), nodes[engine->id].rb);
		struct i915_request *rq = READ_ONCE(ve->request);

		if (!rq) { /* lazily cleanup after another engine handled rq */
			rb_erase_cached(rb, &execlists->virtual);
			RB_CLEAR_NODE(rb);
			rb = rb_first_cached(&execlists->virtual);
			continue;
		}

		if (!virtual_matches(ve, rq, engine)) {
			rb = rb_next(rb);
			continue;
		}

		break;
	}

	/*
	 * If the queue is higher priority than the last
	 * request in the currently active context, submit afresh.
	 * We will resubmit again afterwards in case we need to split
	 * the active context to interject the preemption request,
	 * i.e. we will retrigger preemption following the ack in case
	 * of trouble.
	 */
	last = last_active(execlists);
	if (last) {
		if (need_preempt(engine, last, rb)) {
			GEM_TRACE("%s: preempting last=%llx:%lld, prio=%d, hint=%d\n",
				  engine->name,
				  last->fence.context,
				  last->fence.seqno,
				  last->sched.attr.priority,
				  execlists->queue_priority_hint);
			record_preemption(execlists);

			/*
			 * Don't let the RING_HEAD advance past the breadcrumb
			 * as we unwind (and until we resubmit) so that we do
			 * not accidentally tell it to go backwards.
			 */
			ring_set_paused(engine, 1);

			/*
			 * Note that we have not stopped the GPU at this point,
			 * so we are unwinding the incomplete requests as they
			 * remain inflight and so by the time we do complete
			 * the preemption, some of the unwound requests may
			 * complete!
			 */
			__unwind_incomplete_requests(engine);

			/*
			 * If we need to return to the preempted context, we
			 * need to skip the lite-restore and force it to
			 * reload the RING_TAIL. Otherwise, the HW has a
			 * tendency to ignore us rewinding the TAIL to the
			 * end of an earlier request.
			 */
			last->hw_context->lrc_desc |= CTX_DESC_FORCE_RESTORE;
			last = NULL;
		} else if (need_timeslice(engine, last) &&
			   timer_expired(&engine->execlists.timer)) {
			GEM_TRACE("%s: expired last=%llx:%lld, prio=%d, hint=%d\n",
				  engine->name,
				  last->fence.context,
				  last->fence.seqno,
				  last->sched.attr.priority,
				  execlists->queue_priority_hint);

			ring_set_paused(engine, 1);
			defer_active(engine);

			/*
			 * Unlike for preemption, if we rewind and continue
			 * executing the same context as previously active,
			 * the order of execution will remain the same and
			 * the tail will only advance. We do not need to
			 * force a full context restore, as a lite-restore
			 * is sufficient to resample the monotonic TAIL.
			 *
			 * If we switch to any other context, similarly we
			 * will not rewind TAIL of current context, and
			 * normal save/restore will preserve state and allow
			 * us to later continue executing the same request.
			 */
			last = NULL;
		} else {
			/*
			 * Otherwise if we already have a request pending
			 * for execution after the current one, we can
			 * just wait until the next CS event before
			 * queuing more. In either case we will force a
			 * lite-restore preemption event, but if we wait
			 * we hopefully coalesce several updates into a single
			 * submission.
			 */
			if (!list_is_last(&last->sched.link,
					  &engine->active.requests)) {
				/*
				 * Even if ELSP[1] is occupied and not worthy
				 * of timeslices, our queue might be.
				 */
				if (!execlists->timer.expires &&
				    need_timeslice(engine, last))
					set_timer_ms(&execlists->timer,
						     timeslice(engine));

				return;
			}
		}
	}

	while (rb) { /* XXX virtual is always taking precedence */
		struct virtual_engine *ve =
			rb_entry(rb, typeof(*ve), nodes[engine->id].rb);
		struct i915_request *rq;

		spin_lock(&ve->base.active.lock);

		rq = ve->request;
		if (unlikely(!rq)) { /* lost the race to a sibling */
			spin_unlock(&ve->base.active.lock);
			rb_erase_cached(rb, &execlists->virtual);
			RB_CLEAR_NODE(rb);
			rb = rb_first_cached(&execlists->virtual);
			continue;
		}

		GEM_BUG_ON(rq != ve->request);
		GEM_BUG_ON(rq->engine != &ve->base);
		GEM_BUG_ON(rq->hw_context != &ve->context);

		if (rq_prio(rq) >= queue_prio(execlists)) {
			if (!virtual_matches(ve, rq, engine)) {
				spin_unlock(&ve->base.active.lock);
				rb = rb_next(rb);
				continue;
			}

			if (last && !can_merge_rq(last, rq)) {
				spin_unlock(&ve->base.active.lock);
				return; /* leave this for another */
			}

			GEM_TRACE("%s: virtual rq=%llx:%lld%s, new engine? %s\n",
				  engine->name,
				  rq->fence.context,
				  rq->fence.seqno,
				  i915_request_completed(rq) ? "!" :
				  i915_request_started(rq) ? "*" :
				  "",
				  yesno(engine != ve->siblings[0]));

			ve->request = NULL;
			ve->base.execlists.queue_priority_hint = INT_MIN;
			rb_erase_cached(rb, &execlists->virtual);
			RB_CLEAR_NODE(rb);

			GEM_BUG_ON(!(rq->execution_mask & engine->mask));
			rq->engine = engine;

			if (engine != ve->siblings[0]) {
				u32 *regs = ve->context.lrc_reg_state;
				unsigned int n;

				GEM_BUG_ON(READ_ONCE(ve->context.inflight));

				if (!intel_engine_has_relative_mmio(engine))
					virtual_update_register_offsets(regs,
									engine);

				if (!list_empty(&ve->context.signals))
					virtual_xfer_breadcrumbs(ve, engine);

				/*
				 * Move the bound engine to the top of the list
				 * for future execution. We then kick this
				 * tasklet first before checking others, so that
				 * we preferentially reuse this set of bound
				 * registers.
				 */
				for (n = 1; n < ve->num_siblings; n++) {
					if (ve->siblings[n] == engine) {
						swap(ve->siblings[n],
						     ve->siblings[0]);
						break;
					}
				}

				GEM_BUG_ON(ve->siblings[0] != engine);
			}

			if (__i915_request_submit(rq)) {
				submit = true;
				last = rq;
			}
			i915_request_put(rq);

			/*
			 * Hmm, we have a bunch of virtual engine requests,
			 * but the first one was already completed (thanks
			 * preempt-to-busy!). Keep looking at the veng queue
			 * until we have no more relevant requests (i.e.
			 * the normal submit queue has higher priority).
			 */
			if (!submit) {
				spin_unlock(&ve->base.active.lock);
				rb = rb_first_cached(&execlists->virtual);
				continue;
			}
		}

		spin_unlock(&ve->base.active.lock);
		break;
	}

	while ((rb = rb_first_cached(&execlists->queue))) {
		struct i915_priolist *p = to_priolist(rb);
		struct i915_request *rq, *rn;
		int i;

		priolist_for_each_request_consume(rq, rn, p, i) {
			bool merge = true;

			/*
			 * Can we combine this request with the current port?
			 * It has to be the same context/ringbuffer and not
			 * have any exceptions (e.g. GVT saying never to
			 * combine contexts).
			 *
			 * If we can combine the requests, we can execute both
			 * by updating the RING_TAIL to point to the end of the
			 * second request, and so we never need to tell the
			 * hardware about the first.
			 */
			if (last && !can_merge_rq(last, rq)) {
				/*
				 * If we are on the second port and cannot
				 * combine this request with the last, then we
				 * are done.
				 */
				if (port == last_port)
					goto done;

				/*
				 * We must not populate both ELSP[] with the
				 * same LRCA, i.e. we must submit 2 different
				 * contexts if we submit 2 ELSP.
				 */
				if (last->hw_context == rq->hw_context)
					goto done;

				if (i915_request_has_sentinel(last))
					goto done;

				/*
				 * If GVT overrides us we only ever submit
				 * port[0], leaving port[1] empty. Note that we
				 * also have to be careful that we don't queue
				 * the same context (even though a different
				 * request) to the second port.
				 */
				if (ctx_single_port_submission(last->hw_context) ||
				    ctx_single_port_submission(rq->hw_context))
					goto done;

				merge = false;
			}

			if (__i915_request_submit(rq)) {
				if (!merge) {
					*port = execlists_schedule_in(last, port - execlists->pending);
					port++;
					last = NULL;
				}

				GEM_BUG_ON(last &&
					   !can_merge_ctx(last->hw_context,
							  rq->hw_context));

				submit = true;
				last = rq;
			}
		}

		rb_erase_cached(&p->node, &execlists->queue);
		i915_priolist_free(p);
	}

done:
	/*
	 * Here be a bit of magic! Or sleight-of-hand, whichever you prefer.
	 *
	 * We choose the priority hint such that if we add a request of greater
	 * priority than this, we kick the submission tasklet to decide on
	 * the right order of submitting the requests to hardware. We must
	 * also be prepared to reorder requests as they are in-flight on the
	 * HW. We derive the priority hint then as the first "hole" in
	 * the HW submission ports and if there are no available slots,
	 * the priority of the lowest executing request, i.e. last.
	 *
	 * When we do receive a higher priority request ready to run from the
	 * user, see queue_request(), the priority hint is bumped to that
	 * request triggering preemption on the next dequeue (or subsequent
	 * interrupt for secondary ports).
	 */
	execlists->queue_priority_hint = queue_prio(execlists);
	GEM_TRACE("%s: queue_priority_hint:%d, submit:%s\n",
		  engine->name, execlists->queue_priority_hint,
		  yesno(submit));

	if (submit) {
		*port = execlists_schedule_in(last, port - execlists->pending);
		execlists->switch_priority_hint =
			switch_prio(engine, *execlists->pending);

		/*
		 * Skip if we ended up with exactly the same set of requests,
		 * e.g. trying to timeslice a pair of ordered contexts
		 */
		if (!memcmp(execlists->active, execlists->pending,
			    (port - execlists->pending + 1) * sizeof(*port))) {
			do
				execlists_schedule_out(fetch_and_zero(port));
			while (port-- != execlists->pending);

			goto skip_submit;
		}

		memset(port + 1, 0, (last_port - port) * sizeof(*port));
		execlists_submit_ports(engine);

		set_preempt_timeout(engine);
	} else {
skip_submit:
		ring_set_paused(engine, 0);
	}
}