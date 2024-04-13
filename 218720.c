static bool clone3_args_valid(struct kernel_clone_args *kargs)
{
	/* Verify that no unknown flags are passed along. */
	if (kargs->flags &
	    ~(CLONE_LEGACY_FLAGS | CLONE_CLEAR_SIGHAND | CLONE_INTO_CGROUP))
		return false;

	/*
	 * - make the CLONE_DETACHED bit reuseable for clone3
	 * - make the CSIGNAL bits reuseable for clone3
	 */
	if (kargs->flags & (CLONE_DETACHED | CSIGNAL))
		return false;

	if ((kargs->flags & (CLONE_SIGHAND | CLONE_CLEAR_SIGHAND)) ==
	    (CLONE_SIGHAND | CLONE_CLEAR_SIGHAND))
		return false;

	if ((kargs->flags & (CLONE_THREAD | CLONE_PARENT)) &&
	    kargs->exit_signal)
		return false;

	if (!clone3_stack_valid(kargs))
		return false;

	return true;
}