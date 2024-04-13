static void build_hash_table(struct sum_struct *s)
{
	static uint32 alloc_size;
	int32 i;

	/* Dynamically calculate the hash table size so that the hash load
	 * for big files is about 80%.  A number greater than the traditional
	 * size must be odd or s2 will not be able to span the entire set. */
	tablesize = (uint32)(s->count/8) * 10 + 11;
	if (tablesize < TRADITIONAL_TABLESIZE)
		tablesize = TRADITIONAL_TABLESIZE;
	if (tablesize > alloc_size || tablesize < alloc_size - 16*1024) {
		if (hash_table)
			free(hash_table);
		hash_table = new_array(int32, tablesize);
		if (!hash_table)
			out_of_memory("build_hash_table");
		alloc_size = tablesize;
	}

	memset(hash_table, 0xFF, tablesize * sizeof hash_table[0]);

	if (tablesize == TRADITIONAL_TABLESIZE) {
		for (i = 0; i < s->count; i++) {
			uint32 t = SUM2HASH(s->sums[i].sum1);
			s->sums[i].chain = hash_table[t];
			hash_table[t] = i;
		}
	} else {
		for (i = 0; i < s->count; i++) {
			uint32 t = BIG_SUM2HASH(s->sums[i].sum1);
			s->sums[i].chain = hash_table[t];
			hash_table[t] = i;
		}
	}
}