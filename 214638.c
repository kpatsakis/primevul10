static void matched(int f, struct sum_struct *s, struct map_struct *buf,
		    OFF_T offset, int32 i)
{
	int32 n = (int32)(offset - last_match); /* max value: block_size (int32) */
	int32 j;

	if (verbose > 2 && i >= 0) {
		rprintf(FINFO,
			"match at %.0f last_match=%.0f j=%d len=%ld n=%ld\n",
			(double)offset, (double)last_match, i,
			(long)s->sums[i].len, (long)n);
	}

	send_token(f, i, buf, last_match, n, i < 0 ? 0 : s->sums[i].len);
	data_transfer += n;

	if (i >= 0) {
		stats.matched_data += s->sums[i].len;
		n += s->sums[i].len;
	}

	for (j = 0; j < n; j += CHUNK_SIZE) {
		int32 n1 = MIN(CHUNK_SIZE, n - j);
		sum_update(map_ptr(buf, last_match + j, n1), n1);
	}

	if (i >= 0)
		last_match = offset + s->sums[i].len;
	else
		last_match = offset;

	if (buf && do_progress)
		show_progress(last_match, buf->file_size);
}