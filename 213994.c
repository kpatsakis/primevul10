static GF_Err parse_track_action_params(char *string, TrackAction *action)
{
	char *param = string;
	if (!action || !string) return GF_BAD_PARAM;

	while (param) {
		param = gf_url_colon_suffix(param);
		if (param) {
			*param = 0;
			param++;
#ifndef GPAC_DISABLE_MEDIA_EXPORT
			if (!strncmp("vttnomerge", param, 10)) {
				action->dump_type |= GF_EXPORT_WEBVTT_NOMERGE;
			} else if (!strncmp("layer", param, 5)) {
				action->dump_type |= GF_EXPORT_SVC_LAYER;
			} else if (!strncmp("full", param, 4)) {
				action->dump_type |= GF_EXPORT_NHML_FULL;
			} else if (!strncmp("embedded", param, 8)) {
				action->dump_type |= GF_EXPORT_WEBVTT_META_EMBEDDED;
			} else if (!strncmp("output=", param, 7)) {
				action->out_name = gf_strdup(param+7);
			} else if (!strncmp("src=", param, 4)) {
				action->src_name = gf_strdup(param+4);
			} else if (!strncmp("box=", param, 4)) {
				action->src_name = gf_strdup(param+4);
				action->sample_num = 1;
			} else if (!strncmp("type=", param, 4)) {
				action->udta_type = GF_4CC(param[5], param[6], param[7], param[8]);
			} else if (action->dump_type == GF_EXPORT_RAW_SAMPLES) {
				action->sample_num = atoi(param);
			}
#endif
		}
	}
	if (!strcmp(string, "*")) {
		action->trackID = (u32) -1;
	} else {
		action->trackID = atoi(string);
	}
	return GF_OK;
}