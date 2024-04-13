static struct neighbour **neigh_hash_alloc(unsigned int entries)
{
	unsigned long size = entries * sizeof(struct neighbour *);
	struct neighbour **ret;

	if (size <= PAGE_SIZE) {
		ret = kmalloc(size, GFP_ATOMIC);
	} else {
		ret = (struct neighbour **)
			__get_free_pages(GFP_ATOMIC, get_order(size));
	}
	if (ret)
		memset(ret, 0, size);

	return ret;
}