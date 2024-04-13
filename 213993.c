static u32 create_new_track_action(char *string, TrackAction **actions, u32 *nb_track_act, u32 dump_type)
{
	*actions = (TrackAction *)gf_realloc(*actions, sizeof(TrackAction) * (*nb_track_act+1));
	memset(&(*actions)[*nb_track_act], 0, sizeof(TrackAction) );
	(*actions)[*nb_track_act].act_type = TRAC_ACTION_RAW_EXTRACT;
	(*actions)[*nb_track_act].dump_type = dump_type;
	parse_track_action_params(string, &(*actions)[*nb_track_act]);
	(*nb_track_act)++;
	return dump_type;
}