static void __execlists_kick(struct intel_engine_execlists *execlists)
{
	/* Kick the tasklet for some interrupt coalescing and reset handling */
	tasklet_hi_schedule(&execlists->tasklet);
}