static unsigned long mmap_rnd(void)
{
	unsigned long rnd = 0;

	if (current->flags & PF_RANDOMIZE)
		rnd = (long)get_random_int() & STACK_RND_MASK;

	return rnd << PAGE_SHIFT;
}