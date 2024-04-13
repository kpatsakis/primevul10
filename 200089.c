static inline void shmem_unacct_size(unsigned long flags, loff_t size)
{
	if (flags & VM_ACCOUNT)
		vm_unacct_memory(VM_ACCT(size));
}