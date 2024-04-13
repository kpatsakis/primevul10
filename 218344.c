static void __execlists_submission_tasklet(struct intel_engine_cs *const engine)
{
	lockdep_assert_held(&engine->active.lock);
	if (!engine->execlists.pending[0]) {
		rcu_read_lock(); /* protect peeking at execlists->active */
		execlists_dequeue(engine);
		rcu_read_unlock();
	}
}