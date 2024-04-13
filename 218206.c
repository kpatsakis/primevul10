gen8_csb_parse(const struct intel_engine_execlists *execlists, const u32 *csb)
{
	return *csb & (GEN8_CTX_STATUS_IDLE_ACTIVE | GEN8_CTX_STATUS_PREEMPTED);
}