static int decode_number(struct archive_read* a, struct decode_table* table,
    const uint8_t* p, uint16_t* num)
{
	int i, bits, dist;
	uint16_t bitfield;
	uint32_t pos;
	struct rar5* rar = get_context(a);

	if(ARCHIVE_OK != read_bits_16(rar, p, &bitfield)) {
		return ARCHIVE_EOF;
	}

	bitfield &= 0xfffe;

	if(bitfield < table->decode_len[table->quick_bits]) {
		int code = bitfield >> (16 - table->quick_bits);
		skip_bits(rar, table->quick_len[code]);
		*num = table->quick_num[code];
		return ARCHIVE_OK;
	}

	bits = 15;

	for(i = table->quick_bits + 1; i < 15; i++) {
		if(bitfield < table->decode_len[i]) {
			bits = i;
			break;
		}
	}

	skip_bits(rar, bits);

	dist = bitfield - table->decode_len[bits - 1];
	dist >>= (16 - bits);
	pos = table->decode_pos[bits] + dist;

	if(pos >= table->size)
		pos = 0;

	*num = table->decode_num[pos];
	return ARCHIVE_OK;
}