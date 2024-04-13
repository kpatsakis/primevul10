static inline void put_signal_struct(struct signal_struct *sig)
{
	if (refcount_dec_and_test(&sig->sigcnt))
		free_signal_struct(sig);
}