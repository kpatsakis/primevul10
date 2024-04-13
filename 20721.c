static int read_fragment_table(long long *table_start)
{
	/*
	 * Note on overflow limits:
	 * Size of SBlk.s.fragments is 2^32 (unsigned int)
	 * Max size of bytes is 2^32*8 or 2^35
	 * Max indexes is (2^32*8)/8K or 2^22
	 * Max length is ((2^32*8)/8K)*4 or 2^24 or 16M
	 */
	int res, i;
	long long bytes = SQUASHFS_FRAGMENT_BYTES_2((long long) sBlk.s.fragments);
	int indexes = SQUASHFS_FRAGMENT_INDEXES_2((long long) sBlk.s.fragments);
	int length = SQUASHFS_FRAGMENT_INDEX_BYTES_2((long long) sBlk.s.fragments);
	unsigned int *fragment_table_index;

	/*
	 * The size of the index table (length bytes) should match the
	 * table start and end points
	 */
	if(length != (*table_start- sBlk.s.fragment_table_start)) {
		ERROR("read_ids: Bad inode count in super block\n");
		return FALSE;
	}

	TRACE("read_fragment_table: %d fragments, reading %d fragment indexes "
		"from 0x%llx\n", sBlk.s.fragments, indexes,
		sBlk.s.fragment_table_start);

	fragment_table_index = malloc(length);
	if(fragment_table_index  == NULL)
		EXIT_UNSQUASH("read_fragment_table: failed to allocate "
			"fragment table index\n");

	fragment_table = malloc(bytes);
	if(fragment_table == NULL)
		EXIT_UNSQUASH("read_fragment_table: failed to allocate "
			"fragment table\n");

	if(swap) {
		 unsigned int *sfragment_table_index = malloc(length);

		if(sfragment_table_index == NULL)
			EXIT_UNSQUASH("read_fragment_table: failed to allocate "
				"fragment table index\n");

		 res = read_fs_bytes(fd, sBlk.s.fragment_table_start,
			length, sfragment_table_index);
		if(res == FALSE) {
			ERROR("read_fragment_table: failed to read fragment "
				"table index\n");
			free(sfragment_table_index);
			goto failed;
		}
		SQUASHFS_SWAP_FRAGMENT_INDEXES_2(fragment_table_index,
			sfragment_table_index, indexes);
		free(sfragment_table_index);
	} else {
		res = read_fs_bytes(fd, sBlk.s.fragment_table_start,
			length, fragment_table_index);
		if(res == FALSE) {
			ERROR("read_fragment_table: failed to read fragment "
				"table index\n");
			goto failed;
		}
	}

	for(i = 0; i < indexes; i++) {
		int expected = (i + 1) != indexes ? SQUASHFS_METADATA_SIZE :
					bytes & (SQUASHFS_METADATA_SIZE - 1);
		int length = read_block(fd, fragment_table_index[i], NULL,
			expected, ((char *) fragment_table) + ((long long) i *
			SQUASHFS_METADATA_SIZE));
		TRACE("Read fragment table block %d, from 0x%x, length %d\n", i,
			fragment_table_index[i], length);
		if(length == FALSE) {
			ERROR("read_fragment_table: failed to read fragment "
				"table block\n");
			goto failed;
		}
	}

	if(swap) {
		squashfs_fragment_entry_2 sfragment;
		for(i = 0; i < sBlk.s.fragments; i++) {
			SQUASHFS_SWAP_FRAGMENT_ENTRY_2((&sfragment),
				(&fragment_table[i]));
			memcpy((char *) &fragment_table[i], (char *) &sfragment,
				sizeof(squashfs_fragment_entry_2));
		}
	}

	*table_start = fragment_table_index[0];
	free(fragment_table_index);

	return TRUE;

failed:
	free(fragment_table_index);
	return FALSE;
}