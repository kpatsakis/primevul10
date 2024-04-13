static void cb_event_handler(REvent *ev, int event_type, void *user, void *data) {
	RCore *core = (RCore *)ev->user;
	if (!core->log_events) {
		return;
	}
	REventMeta *rems = data;
	char *str = r_base64_encode_dyn (rems->string, -1);
	switch (event_type) {
	case R_EVENT_META_SET:
		switch (rems->type) {
		case 'C':
			r_core_log_add (ev->user, sdb_fmt (":add-comment 0x%08"PFMT64x" %s\n", rems->addr, str? str: ""));
			break;
		default:
			break;
		}
		break;
	case R_EVENT_META_DEL:
		switch (rems->type) {
		case 'C':
			r_core_log_add (ev->user, sdb_fmt (":del-comment 0x%08"PFMT64x, rems->addr));
			break;
		default:
			r_core_log_add (ev->user, sdb_fmt (":del-comment 0x%08"PFMT64x, rems->addr));
			break;
		}
		break;
	case R_EVENT_META_CLEAR:
		switch (rems->type) {
		case 'C':
			r_core_log_add (ev->user, sdb_fmt (":clear-comments 0x%08"PFMT64x, rems->addr));
			break;
		default:
			r_core_log_add (ev->user, sdb_fmt (":clear-comments 0x%08"PFMT64x, rems->addr));
			break;
		}
		break;
	default:
		// TODO
		break;
	}
	free (str);
}