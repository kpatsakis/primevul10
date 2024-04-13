GF_Err nhmldump_configure_pid(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	u32 cid;
	char *mime=NULL, *name;
	const GF_PropertyValue *p;
	GF_NHMLDumpCtx *ctx = gf_filter_get_udta(filter);

	if (is_remove) {
		ctx->ipid = NULL;
		gf_filter_pid_remove(ctx->opid_nhml);
		gf_filter_pid_remove(ctx->opid_mdia);
		if (ctx->opid_info) gf_filter_pid_remove(ctx->opid_info);
		return GF_OK;
	}
	if (! gf_filter_pid_check_caps(pid))
		return GF_NOT_SUPPORTED;

	p = gf_filter_pid_get_property(pid, GF_PROP_PID_CODECID);
	if (!p) return GF_NOT_SUPPORTED;
	cid = p->value.uint;

	if (ctx->codecid == cid) {
		return GF_OK;
	}
	ctx->codecid = cid;

	if (ctx->codecid<GF_CODECID_LAST_MPEG4_MAPPING) {
		ctx->oti = ctx->codecid;
	} else {
		ctx->oti = gf_codecid_oti(ctx->codecid);
	}

	p = gf_filter_pid_get_property(pid, GF_PROP_PID_STREAM_TYPE);
	ctx->streamtype = p ? p->value.uint : GF_STREAM_UNKNOWN;

	if (!ctx->opid_nhml && !ctx->filep)
		ctx->opid_nhml = gf_filter_pid_new(filter);

	ctx->side_streams_config = GF_FALSE;
	if (ctx->nhmlonly)
		ctx->side_streams_config = GF_TRUE;

	ctx->is_dims = GF_FALSE;
	if ((ctx->codecid == GF_CODECID_DIMS) && ctx->dims) {
		if (ctx->opid_mdia) {
			gf_filter_pid_remove(ctx->opid_mdia);
			ctx->opid_mdia = NULL;
		}
		ctx->is_dims = GF_TRUE;
		ctx->side_streams_config = GF_TRUE;
	}

	//file pointer set, we act as a sink, send play
	if (!ctx->ipid && ctx->filep) {
		GF_FilterEvent evt;
		GF_FEVT_INIT(evt, GF_FEVT_PLAY, pid);
		gf_filter_pid_send_event(pid, &evt);
	}
	ctx->ipid = pid;


	p = gf_filter_pid_get_property(pid, GF_PROP_PID_SAMPLE_RATE);
	ctx->sr = p ? p->value.uint : 0;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_NUM_CHANNELS);
	ctx->chan = p ? p->value.uint : 0;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_AUDIO_FORMAT);
	ctx->bps = p ? gf_audio_fmt_bit_depth(p->value.uint) : 16;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_WIDTH);
	ctx->w = p ? p->value.uint : 0;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_HEIGHT);
	ctx->h = p ? p->value.uint : 0;

	name = (char*) gf_codecid_name(ctx->codecid);
	if (ctx->exporter) {
		if (ctx->w && ctx->h) {
			GF_LOG(GF_LOG_INFO, GF_LOG_AUTHOR, ("Exporting %s - Size %dx%d\n", name, ctx->w, ctx->h));
		} else if (ctx->sr && ctx->chan) {
			GF_LOG(GF_LOG_INFO, GF_LOG_AUTHOR, ("Exporting %s - SampleRate %d %d channels %d bits per sample\n", name, ctx->sr, ctx->chan, ctx->bps));
		} else {
			GF_LOG(GF_LOG_INFO, GF_LOG_AUTHOR, ("Exporting %s\n", name));
		}
	}

	if (ctx->opid_nhml) {
		char *ext;
		mime = ctx->dims ? "application/dims" : "application/x-nhml";
		ext = ctx->dims ? "dims" : "nhml";

		if (ctx->name) {
			char fileName[GF_MAX_PATH+1];
			strncpy(fileName, ctx->name, GF_MAX_PATH);
			fileName[GF_MAX_PATH] = 0;
			name = gf_file_ext_start(fileName);
			if (name) {
				name[0] = 0;
				gf_filter_pid_set_property(ctx->opid_nhml, GF_PROP_PID_OUTPATH, &PROP_STRING(ctx->name) );
			} else {
				strcat(fileName, ".nhml");
				gf_filter_pid_set_property(ctx->opid_nhml, GF_PROP_PID_OUTPATH, &PROP_STRING(fileName) );
			}
		}

		gf_filter_pid_set_property(ctx->opid_nhml, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_FILE) );
		gf_filter_pid_set_property(ctx->opid_nhml, GF_PROP_PID_MIME, &PROP_STRING(mime) );
		gf_filter_pid_set_property(ctx->opid_nhml, GF_PROP_PID_FILE_EXT, &PROP_STRING(ext) );
	}


	ctx->first = GF_TRUE;
	ctx->is_stpp = (cid==GF_CODECID_SUBS_XML) ? GF_TRUE : GF_FALSE;

	p = gf_filter_pid_get_property(pid, GF_PROP_PID_DURATION);
	if (p) ctx->duration = p->value.lfrac;


	if (ctx->opid_nhml)
		gf_filter_pid_set_name(ctx->opid_nhml, "nhml");

	gf_filter_pid_set_framing_mode(pid, GF_TRUE);

	return GF_OK;
}