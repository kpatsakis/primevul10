static GF_Err do_compress_top_boxes(char *inName, char *outName, char *compress_top_boxes, u32 comp_top_box_version, Bool use_lzma)
{
	FILE *in, *out;
	u8 *buf;
	u32 buf_alloc, comp_size, start_offset;
	s32 bytes_comp=0;
	s32 bytes_uncomp=0;
	GF_Err e = GF_OK;
	u64 source_size, dst_size;
	u32 orig_box_overhead;
	u32 final_box_overhead;
	u32 gzip_code = use_lzma ? GF_4CC('l','z','m','a') : GF_4CC('g','z','i','p') ;
	u32 nb_added_box_bytes=0;
	Bool has_mov = GF_FALSE;
	u32 range_idx, nb_ranges=0;
	Bool replace_all = !strcmp(compress_top_boxes, "*");
	Bool requires_byte_ranges=GF_FALSE;
	GF_BitStream *bs_in, *bs_out;
	u32 idx_size=0, nb_moof;
	struct _ranges {
		u32 size, csize;
	} *ranges=NULL;

	if (!outName) {
		fprintf(stderr, "Missing output file name\n");
		return GF_BAD_PARAM;
	}

	in = gf_fopen(inName, "rb");
	if (!in) return GF_URL_ERROR;
	out = gf_fopen(outName, "wb");
	if (!out) return GF_IO_ERR;

	buf_alloc = 4096;
	buf = gf_malloc(buf_alloc);

	bs_in = gf_bs_from_file(in, GF_BITSTREAM_READ);
	source_size = gf_bs_get_size(bs_in);

	bs_out = gf_bs_from_file(out, GF_BITSTREAM_WRITE);

	start_offset = 0;
	nb_moof = 0;
	if (comp_top_box_version==2) {
		u32 i;
		while (gf_bs_available(bs_in)) {
			u32 size = gf_bs_read_u32(bs_in);
			u32 type = gf_bs_read_u32(bs_in);
			const char *b4cc = gf_4cc_to_str(type);
			const char *replace = strstr(compress_top_boxes, b4cc);

			if (start_offset) {
				Bool compress = (replace || replace_all) ? 1 : 0;
				ranges = gf_realloc(ranges, sizeof(struct _ranges)*(nb_ranges+1));
				ranges[nb_ranges].csize = compress;
				ranges[nb_ranges].size = size-8;
				nb_ranges++;
			}
			if (!strcmp(b4cc, "ftyp") || !strcmp(b4cc, "styp")) {
				if (!start_offset) start_offset = (u32) gf_bs_get_position(bs_in) + size-8;
			}
			if (!strcmp(b4cc, "sidx") || !strcmp(b4cc, "ssix")) {
				requires_byte_ranges = GF_TRUE;
			}
			if (!strcmp(b4cc, "moof"))
				nb_moof++;

			gf_bs_skip_bytes(bs_in, size-8);
		}

		gf_bs_seek(bs_in, 0);
		if (buf_alloc<start_offset) {
			buf_alloc = start_offset;
			buf = gf_realloc(buf, buf_alloc);
		}
		gf_bs_read_data(bs_in, buf, start_offset);
		gf_bs_write_data(bs_out, buf, start_offset);

		if (!requires_byte_ranges) {
			nb_ranges = 0;
			gf_free(ranges);
			ranges = NULL;
		}
		idx_size = 8 + 4 + 4;
		for (i=0; i<nb_ranges; i++) {
			idx_size += 1;
			if (ranges[i].size<0xFFFF) {
				idx_size += 2;
				if (ranges[i].csize) idx_size += 2;
			} else {
				idx_size += 4;
				if (ranges[i].csize) idx_size += 4;
			}
			ranges[i].csize = 0;
		}
		i=idx_size;
		while (i) {
			gf_bs_write_u8(bs_out, 0);
			i--;
		}
	}

	range_idx = 0;
	orig_box_overhead = 0;
	final_box_overhead = 0;
	while (gf_bs_available(bs_in)) {
		u32 size = gf_bs_read_u32(bs_in);
		u32 type = gf_bs_read_u32(bs_in);
		const char *b4cc = gf_4cc_to_str(type);
		const u8 *replace = (const u8 *) strstr(compress_top_boxes, b4cc);
		if (!strcmp(b4cc, "moov")) has_mov = GF_TRUE;

		if (!replace && !replace_all) {
			if (ranges) {
				assert(! ranges[range_idx].csize);
				range_idx++;
			}
			gf_bs_write_u32(bs_out, size);
			gf_bs_write_u32(bs_out, type);

			size-=8;
			while (size) {
				u32 nbytes = size;
				if (nbytes>buf_alloc) nbytes=buf_alloc;
				gf_bs_read_data(bs_in, buf, nbytes);
				gf_bs_write_data(bs_out, buf, nbytes);
				size-=nbytes;
			}
			continue;
		}
		orig_box_overhead += size;

		if (comp_top_box_version != 1)
			size-=8;

		if (size>buf_alloc) {
			buf_alloc = size;
			buf = gf_realloc(buf, buf_alloc);
		}
		gf_bs_read_data(bs_in, buf, size);

		if (comp_top_box_version != 1)
			replace+=5;

		comp_size = buf_alloc;

		if (use_lzma) {
			e = gf_lz_compress_payload(&buf, size, &comp_size);
		} else {
			e = gf_gz_compress_payload(&buf, size, &comp_size);
		}
		if (e) break;

		if (comp_size>buf_alloc) {
			buf_alloc = comp_size;
		}
		bytes_uncomp += size;
		bytes_comp += comp_size;
		if (comp_top_box_version==1)
			nb_added_box_bytes +=8;

		//write size
		gf_bs_write_u32(bs_out, comp_size+8);
		//write type
		if (comp_top_box_version==1)
			gf_bs_write_u32(bs_out, gzip_code);
		else
			gf_bs_write_data(bs_out, replace, 4);
		//write data
		gf_bs_write_data(bs_out, buf, comp_size);

		final_box_overhead += 8+comp_size;

		if (ranges) {
			assert(ranges[range_idx].size == size);
			ranges[range_idx].csize = comp_size;
			range_idx++;
		}
	}
	dst_size = gf_bs_get_position(bs_out);

	if (comp_top_box_version==2) {
		u32 i;
		gf_bs_seek(bs_out, start_offset);
		gf_bs_write_u32(bs_out, idx_size);
		gf_bs_write_u32(bs_out, GF_4CC('c','m','a','p'));
		gf_bs_write_u32(bs_out, gzip_code);
		gf_bs_write_u32(bs_out, nb_ranges);
		for (i=0; i<nb_ranges; i++) {
			u32 large_size = ranges[i].size>0xFFFF ? 1 : 0;
			gf_bs_write_int(bs_out, ranges[i].csize ? 1 : 0, 1);
			gf_bs_write_int(bs_out, large_size ? 1 : 0, 1);
			gf_bs_write_int(bs_out, 0, 6);
			large_size = large_size ? 32 : 16;

			gf_bs_write_int(bs_out, ranges[i].size, large_size);
			if (ranges[i].csize)
				gf_bs_write_int(bs_out, ranges[i].csize, large_size);
		}
		final_box_overhead += idx_size;
		nb_added_box_bytes += idx_size;

	}
	gf_bs_del(bs_in);
	gf_bs_del(bs_out);
	gf_fclose(in);
	gf_fclose(out);
	if (e) {
		fprintf(stderr, "Error compressing: %s\n", gf_error_to_string(e));
		return e;
	}

	if (has_mov) {
		u32 i, nb_tracks, nb_samples;
		GF_ISOFile *mov;
		Double rate, new_rate, duration;

		mov = gf_isom_open(inName, GF_ISOM_OPEN_READ, NULL);
		duration = (Double) gf_isom_get_duration(mov);
		duration /= gf_isom_get_timescale(mov);

		nb_samples = 0;
		nb_tracks = gf_isom_get_track_count(mov);
		for (i=0; i<nb_tracks; i++) {
			nb_samples += gf_isom_get_sample_count(mov, i+1);
		}
		gf_isom_close(mov);

		rate = (Double) source_size;
		rate /= duration;
		rate /= 1000;

		new_rate = (Double) dst_size;
		new_rate /= duration;
		new_rate /= 1000;


		fprintf(stderr, "Log format:\nname\torig\tcomp\tgain\tadded_bytes\torate\tcrate\tsamples\tduration\tobbps\tcbbps\n");
		fprintf(stdout, "%s\t%d\t%d\t%g\t%d\t%g\t%g\t%d\t%g\t%g\t%g\n", inName, bytes_uncomp, bytes_comp, ((Double)(bytes_uncomp-bytes_comp)*100)/bytes_uncomp, nb_added_box_bytes, rate, new_rate, nb_samples, duration, ((Double)orig_box_overhead)/nb_samples, ((Double)final_box_overhead)/nb_samples );

		fprintf(stderr, "%s Compressing top-level boxes saved %d bytes out of %d (reduced by %g %%) additional bytes %d original rate %g kbps new rate %g kbps, orig %g box bytes/sample final %g bytes/sample\n", inName, bytes_uncomp-bytes_comp, bytes_uncomp, ((Double)(bytes_uncomp-bytes_comp)*100)/bytes_uncomp, nb_added_box_bytes, rate, new_rate, ((Double)orig_box_overhead)/nb_samples, ((Double)final_box_overhead)/nb_samples );

	} else {
		fprintf(stderr, "Log format:\nname\torig\tcomp\tgain\tadded_bytes\n");
		fprintf(stdout, "%s\t%d\t%d\t%g\t%d\n", inName, bytes_uncomp, bytes_comp, ((Double) (bytes_uncomp - bytes_comp)*100)/(bytes_uncomp), nb_added_box_bytes);

		fprintf(stderr, "%s Compressing top-level boxes saved %d bytes out of %d (reduced by %g %%) additional bytes %d\n", inName, bytes_uncomp-bytes_comp, bytes_uncomp, ((Double)(bytes_uncomp-bytes_comp)*100)/bytes_uncomp, nb_added_box_bytes);

	}
	return GF_OK;
}