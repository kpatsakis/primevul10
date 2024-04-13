static void process_csb(struct intel_engine_cs *engine)
{
	struct intel_engine_execlists * const execlists = &engine->execlists;
	const u32 * const buf = execlists->csb_status;
	const u8 num_entries = execlists->csb_size;
	u8 head, tail;

	/*
	 * As we modify our execlists state tracking we require exclusive
	 * access. Either we are inside the tasklet, or the tasklet is disabled
	 * and we assume that is only inside the reset paths and so serialised.
	 */
	GEM_BUG_ON(!tasklet_is_locked(&execlists->tasklet) &&
		   !reset_in_progress(execlists));
	GEM_BUG_ON(!intel_engine_in_execlists_submission_mode(engine));

	/*
	 * Note that csb_write, csb_status may be either in HWSP or mmio.
	 * When reading from the csb_write mmio register, we have to be
	 * careful to only use the GEN8_CSB_WRITE_PTR portion, which is
	 * the low 4bits. As it happens we know the next 4bits are always
	 * zero and so we can simply masked off the low u8 of the register
	 * and treat it identically to reading from the HWSP (without having
	 * to use explicit shifting and masking, and probably bifurcating
	 * the code to handle the legacy mmio read).
	 */
	head = execlists->csb_head;
	tail = READ_ONCE(*execlists->csb_write);
	GEM_TRACE("%s cs-irq head=%d, tail=%d\n", engine->name, head, tail);
	if (unlikely(head == tail))
		return;

	/*
	 * Hopefully paired with a wmb() in HW!
	 *
	 * We must complete the read of the write pointer before any reads
	 * from the CSB, so that we do not see stale values. Without an rmb
	 * (lfence) the HW may speculatively perform the CSB[] reads *before*
	 * we perform the READ_ONCE(*csb_write).
	 */
	rmb();

	do {
		bool promote;

		if (++head == num_entries)
			head = 0;

		/*
		 * We are flying near dragons again.
		 *
		 * We hold a reference to the request in execlist_port[]
		 * but no more than that. We are operating in softirq
		 * context and so cannot hold any mutex or sleep. That
		 * prevents us stopping the requests we are processing
		 * in port[] from being retired simultaneously (the
		 * breadcrumb will be complete before we see the
		 * context-switch). As we only hold the reference to the
		 * request, any pointer chasing underneath the request
		 * is subject to a potential use-after-free. Thus we
		 * store all of the bookkeeping within port[] as
		 * required, and avoid using unguarded pointers beneath
		 * request itself. The same applies to the atomic
		 * status notifier.
		 */

		GEM_TRACE("%s csb[%d]: status=0x%08x:0x%08x\n",
			  engine->name, head,
			  buf[2 * head + 0], buf[2 * head + 1]);

		if (INTEL_GEN(engine->i915) >= 12)
			promote = gen12_csb_parse(execlists, buf + 2 * head);
		else
			promote = gen8_csb_parse(execlists, buf + 2 * head);
		if (promote) {
			struct i915_request * const *old = execlists->active;

			/* Point active to the new ELSP; prevent overwriting */
			WRITE_ONCE(execlists->active, execlists->pending);
			set_timeslice(engine);

			if (!inject_preempt_hang(execlists))
				ring_set_paused(engine, 0);

			/* cancel old inflight, prepare for switch */
			trace_ports(execlists, "preempted", old);
			while (*old)
				execlists_schedule_out(*old++);

			/* switch pending to inflight */
			GEM_BUG_ON(!assert_pending_valid(execlists, "promote"));
			WRITE_ONCE(execlists->active,
				   memcpy(execlists->inflight,
					  execlists->pending,
					  execlists_num_ports(execlists) *
					  sizeof(*execlists->pending)));

			WRITE_ONCE(execlists->pending[0], NULL);
		} else {
			GEM_BUG_ON(!*execlists->active);

			/* port0 completed, advanced to port1 */
			trace_ports(execlists, "completed", execlists->active);

			/*
			 * We rely on the hardware being strongly
			 * ordered, that the breadcrumb write is
			 * coherent (visible from the CPU) before the
			 * user interrupt and CSB is processed.
			 */
			GEM_BUG_ON(!i915_request_completed(*execlists->active) &&
				   !reset_in_progress(execlists));
			execlists_schedule_out(*execlists->active++);

			GEM_BUG_ON(execlists->active - execlists->inflight >
				   execlists_num_ports(execlists));
		}
	} while (head != tail);

	execlists->csb_head = head;

	/*
	 * Gen11 has proven to fail wrt global observation point between
	 * entry and tail update, failing on the ordering and thus
	 * we see an old entry in the context status buffer.
	 *
	 * Forcibly evict out entries for the next gpu csb update,
	 * to increase the odds that we get a fresh entries with non
	 * working hardware. The cost for doing so comes out mostly with
	 * the wash as hardware, working or not, will need to do the
	 * invalidation before.
	 */
	invalidate_csb_entries(&buf[0], &buf[num_entries - 1]);
}