static int process_block(struct archive_read* a) {
	const uint8_t* p;
	struct rar5* rar = get_context(a);
	int ret;

	/* If we don't have any data to be processed, this most probably means
	 * we need to switch to the next volume. */
	if(rar->main.volume && rar->file.bytes_remaining == 0) {
		ret = advance_multivolume(a);
		if(ret != ARCHIVE_OK)
			return ret;
	}

	if(rar->cstate.block_parsing_finished) {
		ssize_t block_size;
		ssize_t to_skip;
		ssize_t cur_block_size;

		/* The header size won't be bigger than 6 bytes. */
		if(!read_ahead(a, 6, &p)) {
			/* Failed to prefetch data block header. */
			return ARCHIVE_EOF;
		}

		/*
		 * Read block_size by parsing block header. Validate the header
		 * by calculating CRC byte stored inside the header. Size of
		 * the header is not constant (block size can be stored either
		 * in 1 or 2 bytes), that's why block size is left out from the
		 * `compressed_block_header` structure and returned by
		 * `parse_block_header` as the second argument. */

		ret = parse_block_header(a, p, &block_size,
		    &rar->last_block_hdr);
		if(ret != ARCHIVE_OK) {
			return ret;
		}

		/* Skip block header. Next data is huffman tables,
		 * if present. */
		to_skip = sizeof(struct compressed_block_header) +
			bf_byte_count(&rar->last_block_hdr) + 1;

		if(ARCHIVE_OK != consume(a, to_skip))
			return ARCHIVE_EOF;

		rar->file.bytes_remaining -= to_skip;

		/* The block size gives information about the whole block size,
		 * but the block could be stored in split form when using
		 * multi-volume archives. In this case, the block size will be
		 * bigger than the actual data stored in this file. Remaining
		 * part of the data will be in another file. */

		cur_block_size =
			rar5_min(rar->file.bytes_remaining, block_size);

		if(block_size > rar->file.bytes_remaining) {
			/* If current blocks' size is bigger than our data
			 * size, this means we have a multivolume archive.
			 * In this case, skip all base headers until the end
			 * of the file, proceed to next "partXXX.rar" volume,
			 * find its signature, skip all headers up to the first
			 * FILE base header, and continue from there.
			 *
			 * Note that `merge_block` will update the `rar`
			 * context structure quite extensively. */

			ret = merge_block(a, block_size, &p);
			if(ret != ARCHIVE_OK) {
				return ret;
			}

			cur_block_size = block_size;

			/* Current stream pointer should be now directly
			 * *after* the block that spanned through multiple
			 * archive files. `p` pointer should have the data of
			 * the *whole* block (merged from partial blocks
			 * stored in multiple archives files). */
		} else {
			rar->cstate.switch_multivolume = 0;

			/* Read the whole block size into memory. This can take
			 * up to  8 megabytes of memory in theoretical cases.
			 * Might be worth to optimize this and use a standard
			 * chunk of 4kb's. */
			if(!read_ahead(a, 4 + cur_block_size, &p)) {
				/* Failed to prefetch block data. */
				return ARCHIVE_EOF;
			}
		}

		rar->cstate.block_buf = p;
		rar->cstate.cur_block_size = cur_block_size;
		rar->cstate.block_parsing_finished = 0;

		rar->bits.in_addr = 0;
		rar->bits.bit_addr = 0;

		if(bf_is_table_present(&rar->last_block_hdr)) {
			/* Load Huffman tables. */
			ret = parse_tables(a, rar, p);
			if(ret != ARCHIVE_OK) {
				/* Error during decompression of Huffman
				 * tables. */
				return ret;
			}
		}
	} else {
		/* Block parsing not finished, reuse previous memory buffer. */
		p = rar->cstate.block_buf;
	}

	/* Uncompress the block, or a part of it, depending on how many bytes
	 * will be generated by uncompressing the block.
	 *
	 * In case too many bytes will be generated, calling this function
	 * again will resume the uncompression operation. */
	ret = do_uncompress_block(a, p);
	if(ret != ARCHIVE_OK) {
		return ret;
	}

	if(rar->cstate.block_parsing_finished &&
	    rar->cstate.switch_multivolume == 0 &&
	    rar->cstate.cur_block_size > 0)
	{
		/* If we're processing a normal block, consume the whole
		 * block. We can do this because we've already read the whole
		 * block to memory. */
		if(ARCHIVE_OK != consume(a, rar->cstate.cur_block_size))
			return ARCHIVE_FATAL;

		rar->file.bytes_remaining -= rar->cstate.cur_block_size;
	} else if(rar->cstate.switch_multivolume) {
		/* Don't consume the block if we're doing multivolume
		 * processing. The volume switching function will consume
		 * the proper count of bytes instead. */
		rar->cstate.switch_multivolume = 0;
	}

	return ARCHIVE_OK;
}