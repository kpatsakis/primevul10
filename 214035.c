GF_Err nhmldump_process(GF_Filter *filter)
{
	GF_NHMLDumpCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck;
	char *data;
	u32 pck_size;

	if (!ctx->side_streams_config) {
		return nhmldump_config_side_stream(filter, ctx);
	}

	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (!pck) {
		if (gf_filter_pid_is_eos(ctx->ipid)) {
			if (ctx->bs_w && ctx->szRootName) {
				char nhml[1024];
				u32 size;
				gf_bs_reassign_buffer(ctx->bs_w, ctx->nhml_buffer, ctx->nhml_buffer_size);
				sprintf(nhml, "</%s>\n", ctx->szRootName);
				gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));

				gf_bs_get_content_no_truncate(ctx->bs_w, &ctx->nhml_buffer, &size, &ctx->nhml_buffer_size);

				if (ctx->filep) {
					gf_fwrite(ctx->nhml_buffer, size, ctx->filep);
				} else {
					GF_FilterPacket *dst_pck;
					u8 *output;
					dst_pck = gf_filter_pck_new_alloc(ctx->opid_nhml, size, &output);
					memcpy(output, ctx->nhml_buffer, size);
					gf_filter_pck_set_framing(dst_pck, GF_FALSE, GF_TRUE);
					gf_filter_pck_send(dst_pck);
				}
				ctx->szRootName = NULL;
			}
			if (ctx->opid_nhml) gf_filter_pid_set_eos(ctx->opid_nhml);
			if (ctx->opid_mdia) gf_filter_pid_set_eos(ctx->opid_mdia);
			if (ctx->opid_info) gf_filter_pid_set_eos(ctx->opid_info);
			return GF_EOS;
		}
		return GF_OK;
	}

	if (!ctx->bs_w) ctx->bs_w = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	else gf_bs_reassign_buffer(ctx->bs_w, ctx->nhml_buffer, ctx->nhml_buffer_size);

	if (ctx->first) {
		nhmldump_send_header(ctx);
		gf_bs_reassign_buffer(ctx->bs_w, ctx->nhml_buffer, ctx->nhml_buffer_size);
	}

	//get media data
	data = (char *) gf_filter_pck_get_data(pck, &pck_size);

	//send data
	if (ctx->is_dims) {
		nhmldump_send_dims(ctx, data, pck_size, pck);
	} else {
		nhmldump_send_frame(ctx, data, pck_size, pck);
	}
	ctx->first = GF_FALSE;


	if (ctx->exporter) {
		u32 timescale = gf_filter_pck_get_timescale(pck);
		u64 ts = gf_filter_pck_get_cts(pck);
		gf_set_progress("Exporting", ts*ctx->duration.den, ctx->duration.num*timescale);
	}

	gf_filter_pid_drop_packet(ctx->ipid);

	return GF_OK;
}