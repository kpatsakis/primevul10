static int create_decode_tables(uint8_t* bit_length,
    struct decode_table* table, int size)
{
	int code, upper_limit = 0, i, lc[16];
	uint32_t decode_pos_clone[rar5_countof(table->decode_pos)];
	ssize_t cur_len, quick_data_size;

	memset(&lc, 0, sizeof(lc));
	memset(table->decode_num, 0, sizeof(table->decode_num));
	table->size = size;
	table->quick_bits = size == HUFF_NC ? 10 : 7;

	for(i = 0; i < size; i++) {
		lc[bit_length[i] & 15]++;
	}

	lc[0] = 0;
	table->decode_pos[0] = 0;
	table->decode_len[0] = 0;

	for(i = 1; i < 16; i++) {
		upper_limit += lc[i];

		table->decode_len[i] = upper_limit << (16 - i);
		table->decode_pos[i] = table->decode_pos[i - 1] + lc[i - 1];

		upper_limit <<= 1;
	}

	memcpy(decode_pos_clone, table->decode_pos, sizeof(decode_pos_clone));

	for(i = 0; i < size; i++) {
		uint8_t clen = bit_length[i] & 15;
		if(clen > 0) {
			int last_pos = decode_pos_clone[clen];
			table->decode_num[last_pos] = i;
			decode_pos_clone[clen]++;
		}
	}

	quick_data_size = (int64_t)1 << table->quick_bits;
	cur_len = 1;
	for(code = 0; code < quick_data_size; code++) {
		int bit_field = code << (16 - table->quick_bits);
		int dist, pos;

		while(cur_len < rar5_countof(table->decode_len) &&
				bit_field >= table->decode_len[cur_len]) {
			cur_len++;
		}

		table->quick_len[code] = (uint8_t) cur_len;

		dist = bit_field - table->decode_len[cur_len - 1];
		dist >>= (16 - cur_len);

		pos = table->decode_pos[cur_len & 15] + dist;
		if(cur_len < rar5_countof(table->decode_pos) && pos < size) {
			table->quick_num[code] = table->decode_num[pos];
		} else {
			table->quick_num[code] = 0;
		}
	}

	return ARCHIVE_OK;
}