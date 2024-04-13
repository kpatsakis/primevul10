static void nhmldump_send_dims(GF_NHMLDumpCtx *ctx, char *data, u32 data_size, GF_FilterPacket *pck)
{
	char nhml[1024];
	u32 size;
	u8 *output;
	GF_FilterPacket *dst_pck;
	u64 dts = gf_filter_pck_get_dts(pck);
	u64 cts = gf_filter_pck_get_cts(pck);

	if (dts==GF_FILTER_NO_TS) dts = cts;
	if (cts==GF_FILTER_NO_TS) cts = dts;

	if (!ctx->bs_r) ctx->bs_r = gf_bs_new(data, data_size, GF_BITSTREAM_READ);
	else gf_bs_reassign_buffer(ctx->bs_r, data, data_size);

	while (gf_bs_available(ctx->bs_r)) {
		u64 pos = gf_bs_get_position(ctx->bs_r);
		size = gf_bs_read_u16(ctx->bs_r);
		u8 flags = gf_bs_read_u8(ctx->bs_r);
		u8 prev;

		if (pos+size+2 > data_size)
			break;

		prev = data[pos+2+size];
		data[pos+2+size] = 0;


		sprintf(nhml, "<DIMSUnit time=\""LLU"\"", cts);
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));

		/*DIMS flags*/
		if (flags & GF_DIMS_UNIT_S) {
			sprintf(nhml, " is-Scene=\"yes\"");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		if (flags & GF_DIMS_UNIT_M) {
			sprintf(nhml, " is-RAP=\"yes\"");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		if (flags & GF_DIMS_UNIT_I) {
			sprintf(nhml, " is-redundant=\"yes\"");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		if (flags & GF_DIMS_UNIT_D) {
			sprintf(nhml, " redundant-exit=\"yes\"");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		if (flags & GF_DIMS_UNIT_P) {
			sprintf(nhml, " priority=\"high\"");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		if (flags & GF_DIMS_UNIT_C) {
			sprintf(nhml, " compressed=\"yes\"");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		sprintf(nhml, ">");
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		if (ctx->uncompress && (flags & GF_DIMS_UNIT_C)) {
#ifndef GPAC_DISABLE_ZLIB
			char svg_data[2049];
			int err;
			u32 done = 0;
			z_stream d_stream;
			d_stream.zalloc = (alloc_func)0;
			d_stream.zfree = (free_func)0;
			d_stream.opaque = (voidpf)0;
			d_stream.next_in  = (Bytef*) data+pos+3;
			d_stream.avail_in = size-1;
			d_stream.next_out = (Bytef*)svg_data;
			d_stream.avail_out = 2048;

			err = inflateInit(&d_stream);
			if (err == Z_OK) {
				while ((s32) d_stream.total_in < size-1) {
					err = inflate(&d_stream, Z_NO_FLUSH);
					if (err < Z_OK) break;
					svg_data[d_stream.total_out - done] = 0;
					gf_bs_write_data(ctx->bs_w, svg_data, (u32) strlen(svg_data));

					if (err== Z_STREAM_END) break;
					done = (u32) d_stream.total_out;
					d_stream.avail_out = 2048;
					d_stream.next_out = (Bytef*)svg_data;
				}
				inflateEnd(&d_stream);
			}
#else
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("Error: your version of GPAC was compiled with no libz support."));
			gf_bs_del(ctx->bs_r);
			return;
#endif
		} else {
			gf_bs_write_data(ctx->bs_w, data+pos+3, size-1);
		}
		sprintf(nhml, "</DIMSUnit>\n");
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));

		data[pos+2+size] = prev;
		gf_bs_skip_bytes(ctx->bs_r, size-1);
	}

	gf_bs_get_content_no_truncate(ctx->bs_w, &ctx->nhml_buffer, &size, &ctx->nhml_buffer_size);

	if (ctx->filep) {
		gf_fwrite(ctx->nhml_buffer, size, ctx->filep);
		return;
	}

	dst_pck = gf_filter_pck_new_alloc(ctx->opid_nhml, size, &output);
	memcpy(output, ctx->nhml_buffer, size);
	gf_filter_pck_set_framing(dst_pck, GF_FALSE, GF_FALSE);
	gf_filter_pck_send(dst_pck);
}