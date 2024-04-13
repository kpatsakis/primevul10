void match_sums(int f, struct sum_struct *s, struct map_struct *buf, OFF_T len)
{
	char file_sum[MAX_DIGEST_LEN];
	int sum_len;

	last_match = 0;
	false_alarms = 0;
	hash_hits = 0;
	matches = 0;
	data_transfer = 0;

	sum_init(checksum_seed);

	if (append_mode > 0) {
		if (append_mode == 2) {
			OFF_T j = 0;
			for (j = CHUNK_SIZE; j < s->flength; j += CHUNK_SIZE) {
				if (buf && do_progress)
					show_progress(last_match, buf->file_size);
				sum_update(map_ptr(buf, last_match, CHUNK_SIZE),
					   CHUNK_SIZE);
				last_match = j;
			}
			if (last_match < s->flength) {
				int32 n = (int32)(s->flength - last_match);
				if (buf && do_progress)
					show_progress(last_match, buf->file_size);
				sum_update(map_ptr(buf, last_match, n), n);
			}
		}
		last_match = s->flength;
		s->count = 0;
	}

	if (len > 0 && s->count > 0) {
		build_hash_table(s);

		if (verbose > 2)
			rprintf(FINFO,"built hash table\n");

		hash_search(f, s, buf, len);

		if (verbose > 2)
			rprintf(FINFO,"done hash search\n");
	} else {
		OFF_T j;
		/* by doing this in pieces we avoid too many seeks */
		for (j = last_match + CHUNK_SIZE; j < len; j += CHUNK_SIZE)
			matched(f, s, buf, j, -2);
		matched(f, s, buf, len, -1);
	}

	sum_len = sum_end(file_sum);
	/* If we had a read error, send a bad checksum. */
	if (buf && buf->status != 0)
		file_sum[0]++;

	if (verbose > 2)
		rprintf(FINFO,"sending file_sum\n");
	write_buf(f, file_sum, sum_len);

	if (verbose > 2)
		rprintf(FINFO, "false_alarms=%d hash_hits=%d matches=%d\n",
			false_alarms, hash_hits, matches);

	total_hash_hits += hash_hits;
	total_false_alarms += false_alarms;
	total_matches += matches;
	stats.literal_data += data_transfer;
}