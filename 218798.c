static inline bool clone3_stack_valid(struct kernel_clone_args *kargs)
{
	if (kargs->stack == 0) {
		if (kargs->stack_size > 0)
			return false;
	} else {
		if (kargs->stack_size == 0)
			return false;

		if (!access_ok((void __user *)kargs->stack, kargs->stack_size))
			return false;

#if !defined(CONFIG_STACK_GROWSUP) && !defined(CONFIG_IA64)
		kargs->stack += kargs->stack_size;
#endif
	}

	return true;
}