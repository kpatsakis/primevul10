gen12_csb_parse(const struct intel_engine_execlists *execlists, const u32 *csb)
{
	u32 lower_dw = csb[0];
	u32 upper_dw = csb[1];
	bool ctx_to_valid = GEN12_CSB_CTX_VALID(lower_dw);
	bool ctx_away_valid = GEN12_CSB_CTX_VALID(upper_dw);
	bool new_queue = lower_dw & GEN12_CTX_STATUS_SWITCHED_TO_NEW_QUEUE;

	/*
	 * The context switch detail is not guaranteed to be 5 when a preemption
	 * occurs, so we can't just check for that. The check below works for
	 * all the cases we care about, including preemptions of WAIT
	 * instructions and lite-restore. Preempt-to-idle via the CTRL register
	 * would require some extra handling, but we don't support that.
	 */
	if (!ctx_away_valid || new_queue) {
		GEM_BUG_ON(!ctx_to_valid);
		return true;
	}

	/*
	 * switch detail = 5 is covered by the case above and we do not expect a
	 * context switch on an unsuccessful wait instruction since we always
	 * use polling mode.
	 */
	GEM_BUG_ON(GEN12_CTX_SWITCH_DETAIL(upper_dw));
	return false;
}