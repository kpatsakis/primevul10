static long long *salloc_index_table(int indexes)
{
	static long long *alloc_table = NULL;
	static int alloc_size = 0;
	int length = indexes * sizeof(long long);

	if(alloc_size < length || length == 0) {
		long long *table = realloc(alloc_table, length);

		if(table == NULL && length !=0 )
			EXIT_UNSQUASH("alloc_index_table: failed to allocate "
				"index table\n");

		alloc_table = table;
		alloc_size = length;
	}

	return alloc_table;
}