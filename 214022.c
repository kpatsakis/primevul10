GF_Err nhmldump_config_side_stream(GF_Filter *filter, GF_NHMLDumpCtx *ctx)
{
	char *mime=NULL, *name;
	char fileName[GF_MAX_PATH+1];
	const GF_PropertyValue *p;
	GF_FileIO *gfio = NULL;

	if (ctx->name) {
		strncpy(fileName, ctx->name, GF_MAX_PATH);
		fileName[GF_MAX_PATH] = 0;
	} else {
		char *url = gf_filter_pid_get_destination(ctx->opid_nhml);
		if (url) {
			if (!strncmp(url, "gfio://", 7)) {
				gfio = gf_fileio_from_url(url);
				strncpy(fileName, gf_fileio_translate_url(url), GF_MAX_PATH);
			} else {
				strncpy(fileName, url, GF_MAX_PATH);
			}
			fileName[GF_MAX_PATH] = 0;
			gf_free(url);
 		} else {
			strcpy(fileName, "dump");
		}
	}
	name = gf_file_ext_start(fileName);
	if (name) {
		name[0] = 0;
	}

	if (!ctx->opid_mdia && !ctx->nhmlonly)
		ctx->opid_mdia = gf_filter_pid_new(filter);

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_DECODER_CONFIG);
	if (p) {
		ctx->dcfg = p->value.data.ptr;
		ctx->dcfg_size = p->value.data.size;

		if (!ctx->opid_info && !ctx->nhmlonly) {
			ctx->opid_info = gf_filter_pid_new(filter);
		}

	} else if (ctx->opid_info) {
		gf_filter_pid_remove(ctx->opid_info);
	}
	if (ctx->info_file) gf_free(ctx->info_file);
	ctx->info_file = NULL;

	if (ctx->opid_mdia) {
		GF_Err e;
		char *res_name;
		gf_filter_pid_set_property(ctx->opid_mdia, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_FILE) );
		gf_filter_pid_set_property(ctx->opid_mdia, GF_PROP_PID_MIME, &PROP_STRING(mime) );

		name = gf_file_ext_start(fileName);
		if (name) name[0] = 0;
		strcat(fileName, ".media");
		if (gfio) {
			res_name = (char *) gf_fileio_factory(gfio, gf_file_basename(fileName) );
		} else {
			res_name = fileName;
		}
		if (!ctx->exporter) {
			gf_filter_pid_set_property(ctx->opid_mdia, GF_PROP_PID_OUTPATH, &PROP_STRING(res_name) );
		}

		if (ctx->media_file) gf_free(ctx->media_file);
		ctx->media_file = gf_strdup(fileName);
		gf_filter_pid_set_property(ctx->opid_mdia, GF_PROP_PID_FILE_EXT, &PROP_STRING("media") );

		if (!ctx->exporter) {
			GF_Filter *o_media = gf_filter_connect_destination(filter, res_name, &e);
			if (o_media) gf_filter_set_source(o_media, filter, NULL);
		}
	}

	if (ctx->opid_info) {
		char *res_name;
		GF_Err e;
		gf_filter_pid_set_property(ctx->opid_info, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_FILE) );
		gf_filter_pid_set_property(ctx->opid_info, GF_PROP_PID_MIME, &PROP_STRING(mime) );

		name = gf_file_ext_start(fileName);
		if (name) name[0] = 0;
		strcat(fileName, ".info");
		if (gfio) {
			res_name = (char *) gf_fileio_factory(gfio, gf_file_basename(fileName) );
		} else {
			res_name = fileName;
		}
		if (!ctx->exporter) {
			gf_filter_pid_set_property(ctx->opid_info, GF_PROP_PID_OUTPATH, &PROP_STRING(res_name) );
		}

		if (ctx->info_file) gf_free(ctx->info_file);
		ctx->info_file = gf_strdup(fileName);
		gf_filter_pid_set_property(ctx->opid_info, GF_PROP_PID_FILE_EXT, &PROP_STRING("info") );

		if (!ctx->exporter) {
			GF_Filter *o_info = gf_filter_connect_destination(filter, res_name, &e);
			if (o_info) gf_filter_set_source(o_info, filter, NULL);
		}
	}

	if (ctx->opid_mdia)
		gf_filter_pid_set_name(ctx->opid_mdia, "media");
	if (ctx->opid_info)
		gf_filter_pid_set_name(ctx->opid_info, "info");

	ctx->side_streams_config = GF_TRUE;
	return GF_OK;
}