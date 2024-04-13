static u64 execlists_update_context(struct i915_request *rq)
{
	struct intel_context *ce = rq->hw_context;
	u64 desc = ce->lrc_desc;
	u32 tail;

	/*
	 * WaIdleLiteRestore:bdw,skl
	 *
	 * We should never submit the context with the same RING_TAIL twice
	 * just in case we submit an empty ring, which confuses the HW.
	 *
	 * We append a couple of NOOPs (gen8_emit_wa_tail) after the end of
	 * the normal request to be able to always advance the RING_TAIL on
	 * subsequent resubmissions (for lite restore). Should that fail us,
	 * and we try and submit the same tail again, force the context
	 * reload.
	 */
	tail = intel_ring_set_tail(rq->ring, rq->tail);
	if (unlikely(ce->lrc_reg_state[CTX_RING_TAIL] == tail))
		desc |= CTX_DESC_FORCE_RESTORE;
	ce->lrc_reg_state[CTX_RING_TAIL] = tail;
	rq->tail = rq->wa_tail;

	/*
	 * Make sure the context image is complete before we submit it to HW.
	 *
	 * Ostensibly, writes (including the WCB) should be flushed prior to
	 * an uncached write such as our mmio register access, the empirical
	 * evidence (esp. on Braswell) suggests that the WC write into memory
	 * may not be visible to the HW prior to the completion of the UC
	 * register write and that we may begin execution from the context
	 * before its image is complete leading to invalid PD chasing.
	 *
	 * Furthermore, Braswell, at least, wants a full mb to be sure that
	 * the writes are coherent in memory (visible to the GPU) prior to
	 * execution, and not just visible to other CPUs (as is the result of
	 * wmb).
	 */
	mb();

	/* Wa_1607138340:tgl */
	if (IS_TGL_REVID(rq->i915, TGL_REVID_A0, TGL_REVID_A0))
		desc |= CTX_DESC_FORCE_RESTORE;

	ce->lrc_desc &= ~CTX_DESC_FORCE_RESTORE;
	return desc;
}