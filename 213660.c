GF_Err av1dmx_configure_pid(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	const GF_PropertyValue *p;
	GF_AV1DmxCtx *ctx = gf_filter_get_udta(filter);

	if (is_remove) {
		ctx->ipid = NULL;
		if (ctx->opid) {
			gf_filter_pid_remove(ctx->opid);
			ctx->opid = NULL;
		}
		return GF_OK;
	}
	if (! gf_filter_pid_check_caps(pid))
		return GF_NOT_SUPPORTED;

	ctx->ipid = pid;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_TIMESCALE);
	if (p) ctx->timescale = p->value.uint;
	ctx->state.mem_mode = GF_TRUE;
	if (ctx->timescale && !ctx->opid) {
		ctx->opid = gf_filter_pid_new(filter);
		gf_filter_pid_copy_properties(ctx->opid, ctx->ipid);
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED, NULL);
	}
	if (ctx->timescale) {
		//if we have a FPS prop, use it
		p = gf_filter_pid_get_property(pid, GF_PROP_PID_FPS);
		if (p) ctx->cur_fps = p->value.frac;
	}
	return GF_OK;
}