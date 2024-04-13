static bool find_autocomplete(RCore *core, RLineCompletion *completion, RLineBuffer *buf) {
	RCoreAutocomplete* child = NULL;
	RCoreAutocomplete* parent = core->autocomplete;
	const char* p = buf->data;
	if (!*p) {
		return false;
	}
	char arg[256];
	arg[0] = 0;
	while (*p) {
		const char* e = r_str_trim_head_wp (p);
		if (!e || (e - p) >= 256 || e == p) {
			return false;
		}
		memcpy (arg, p, e - p);
		arg[e - p] = 0;
		child = r_core_autocomplete_find (parent, arg, false);
		if (child && child->length < buf->length && p[child->length] == ' ') {
			// if is spaced then i can provide the
			// next subtree as suggestion..
			p = r_str_trim_head_ro (p + child->length);
			if (child->type == R_CORE_AUTOCMPLT_OPTN) {
				continue;
			}
			parent = child;
		} else {
			break;
		}
	}
	int i;
	/* if something went wrong this will prevent bad behavior */
	r_line_completion_clear (completion);
	switch (parent->type) {
	case R_CORE_AUTOCMPLT_SEEK:
		autocomplete_functions (core, completion, p);
	case R_CORE_AUTOCMPLT_FLAG:
		autocomplete_flags (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_FLSP:
		autocomplete_flagspaces (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_FCN:
		autocomplete_functions (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_ZIGN:
		autocomplete_zignatures (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_EVAL:
		autocomplete_evals (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_PRJT:
		autocomplete_project (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_MINS:
		autocomplete_minus (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_BRKP:
		autocomplete_breakpoints (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_MACR:
		autocomplete_macro (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_MS:
		autocomplete_ms_file(core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_FILE:
		autocomplete_file (completion, p);
		break;
	case R_CORE_AUTOCMPLT_THME:
		autocomplete_theme (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_SDB:
		autocomplete_sdb (core, completion, p);
		break;
	case R_CORE_AUTOCMPLT_OPTN:
		// handled before
		break;
	default:
		if (r_config_get_i (core->config, "cfg.newtab")) {
			RCmdDescriptor *desc = &core->root_cmd_descriptor;
			for (i = 0; arg[i] && desc; i++) {
				ut8 c = arg[i];
				desc = c < R_ARRAY_SIZE (desc->sub) ? desc->sub[c] : NULL;
			}
			if (desc && desc->help_msg) {
				r_core_cmd_help (core, desc->help_msg);
				r_cons_flush ();
				return true;
			}
			// fallback to command listing
		}
		int length = strlen (arg);
		for (i = 0; i < parent->n_subcmds; i++) {
			if (!strncmp (arg, parent->subcmds[i]->cmd, length)) {
				r_line_completion_push (completion, parent->subcmds[i]->cmd);
			}
		}
		break;
	}
	return true;
}