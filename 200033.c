static inline int shmem_acct_block(unsigned long flags)
{
	return (flags & VM_ACCOUNT)?
		0: security_vm_enough_memory(VM_ACCT(PAGE_CACHE_SIZE));
}