noinline static int copy_clone_args_from_user(struct kernel_clone_args *kargs,
					      struct clone_args __user *uargs,
					      size_t usize)
{
	int err;
	struct clone_args args;
	pid_t *kset_tid = kargs->set_tid;

	BUILD_BUG_ON(offsetofend(struct clone_args, tls) !=
		     CLONE_ARGS_SIZE_VER0);
	BUILD_BUG_ON(offsetofend(struct clone_args, set_tid_size) !=
		     CLONE_ARGS_SIZE_VER1);
	BUILD_BUG_ON(offsetofend(struct clone_args, cgroup) !=
		     CLONE_ARGS_SIZE_VER2);
	BUILD_BUG_ON(sizeof(struct clone_args) != CLONE_ARGS_SIZE_VER2);

	if (unlikely(usize > PAGE_SIZE))
		return -E2BIG;
	if (unlikely(usize < CLONE_ARGS_SIZE_VER0))
		return -EINVAL;

	err = copy_struct_from_user(&args, sizeof(args), uargs, usize);
	if (err)
		return err;

	if (unlikely(args.set_tid_size > MAX_PID_NS_LEVEL))
		return -EINVAL;

	if (unlikely(!args.set_tid && args.set_tid_size > 0))
		return -EINVAL;

	if (unlikely(args.set_tid && args.set_tid_size == 0))
		return -EINVAL;

	/*
	 * Verify that higher 32bits of exit_signal are unset and that
	 * it is a valid signal
	 */
	if (unlikely((args.exit_signal & ~((u64)CSIGNAL)) ||
		     !valid_signal(args.exit_signal)))
		return -EINVAL;

	if ((args.flags & CLONE_INTO_CGROUP) &&
	    (args.cgroup > INT_MAX || usize < CLONE_ARGS_SIZE_VER2))
		return -EINVAL;

	*kargs = (struct kernel_clone_args){
		.flags		= args.flags,
		.pidfd		= u64_to_user_ptr(args.pidfd),
		.child_tid	= u64_to_user_ptr(args.child_tid),
		.parent_tid	= u64_to_user_ptr(args.parent_tid),
		.exit_signal	= args.exit_signal,
		.stack		= args.stack,
		.stack_size	= args.stack_size,
		.tls		= args.tls,
		.set_tid_size	= args.set_tid_size,
		.cgroup		= args.cgroup,
	};

	if (args.set_tid &&
		copy_from_user(kset_tid, u64_to_user_ptr(args.set_tid),
			(kargs->set_tid_size * sizeof(pid_t))))
		return -EFAULT;

	kargs->set_tid = kset_tid;

	return 0;
}