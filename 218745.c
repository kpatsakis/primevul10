static __poll_t pidfd_poll(struct file *file, struct poll_table_struct *pts)
{
	struct pid *pid = file->private_data;
	__poll_t poll_flags = 0;

	poll_wait(file, &pid->wait_pidfd, pts);

	/*
	 * Inform pollers only when the whole thread group exits.
	 * If the thread group leader exits before all other threads in the
	 * group, then poll(2) should block, similar to the wait(2) family.
	 */
	if (thread_group_exited(pid))
		poll_flags = EPOLLIN | EPOLLRDNORM;

	return poll_flags;
}