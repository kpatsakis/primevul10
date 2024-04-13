R_API void r_core_autocomplete(R_NULLABLE RCore *core, RLineCompletion *completion, RLineBuffer *buf, RLinePromptType prompt_type) {
	if (!core) {
		autocomplete_default (core, completion, buf);
		return;
	}
	r_line_completion_clear (completion);
	char *pipe = strchr (buf->data, '>');
	char *ptr = strchr (buf->data, '@');
	if (pipe && strchr (pipe + 1, ' ') && buf->data + buf->index >= pipe) {
		autocompleteFilename (completion, buf, NULL, 1);
	} else if (ptr && strchr (ptr + 1, ' ') && buf->data + buf->index >= ptr) {
		int sdelta, n;
		ptr = (char *)r_str_trim_head_ro (ptr + 1);
		n = strlen (ptr);//(buf->data+sdelta);
		sdelta = (int)(size_t)(ptr - buf->data);
		r_flag_foreach_prefix (core->flags, buf->data + sdelta, n, add_argv, completion);
	} else if (!strncmp (buf->data, "#!pipe ", 7)) {
		if (strchr (buf->data + 7, ' ')) {
			autocompleteFilename (completion, buf, NULL, 2);
		} else {
			int chr = 7;
			ADDARG ("node");
			ADDARG ("vala");
			ADDARG ("ruby");
			ADDARG ("newlisp");
			ADDARG ("perl");
			ADDARG ("python");
		}
	} else if (!strncmp (buf->data, "ec ", 3)) {
		if (strchr (buf->data + 3, ' ')) {
			autocompleteFilename (completion, buf, NULL, 2);
		} else {
			int chr = 3;
			ADDARG("comment")
			ADDARG("usrcmt")
			ADDARG("args")
			ADDARG("fname")
			ADDARG("floc")
			ADDARG("fline")
			ADDARG("flag")
			ADDARG("label")
			ADDARG("help")
			ADDARG("flow")
			ADDARG("prompt")
			ADDARG("offset")
			ADDARG("input")
			ADDARG("invalid")
			ADDARG("other")
			ADDARG("b0x00")
			ADDARG("b0x7f")
			ADDARG("b0xff")
			ADDARG("math")
			ADDARG("bin")
			ADDARG("btext")
			ADDARG("push")
			ADDARG("pop")
			ADDARG("crypto")
			ADDARG("jmp")
			ADDARG("cjmp")
			ADDARG("call")
			ADDARG("nop")
			ADDARG("ret")
			ADDARG("trap")
			ADDARG("swi")
			ADDARG("cmp")
			ADDARG("reg")
			ADDARG("creg")
			ADDARG("num")
			ADDARG("mov")
			ADDARG("func_var")
			ADDARG("func_var_type")
			ADDARG("func_var_addr")
			ADDARG("widget_bg")
			ADDARG("widget_sel")
			ADDARG("ai.read")
			ADDARG("ai.write")
			ADDARG("ai.exec")
			ADDARG("ai.seq")
			ADDARG("ai.ascii")
			ADDARG("ai.unmap")
			ADDARG("graph.box")
			ADDARG("graph.box2")
			ADDARG("graph.box3")
			ADDARG("graph.box4")
			ADDARG("graph.true")
			ADDARG("graph.false")
			ADDARG("graph.trufae")
			ADDARG("graph.current")
			ADDARG("graph.traced")
			ADDARG("gui.cflow")
			ADDARG("gui.dataoffset")
			ADDARG("gui.background")
			ADDARG("gui.alt_background")
			ADDARG("gui.border")
		}
	} else if (!strncmp (buf->data, "pf.", 3)
	|| !strncmp (buf->data, "pf*.", 4)
	|| !strncmp (buf->data, "pfd.", 4)
	|| !strncmp (buf->data, "pfv.", 4)
	|| !strncmp (buf->data, "pfj.", 4)) {
		char pfx[2];
		int chr = (buf->data[2]=='.')? 3: 4;
		if (chr == 4) {
			pfx[0] = buf->data[2];
			pfx[1] = 0;
		} else {
			*pfx = 0;
		}
		SdbList *sls = sdb_foreach_list (core->print->formats, false);
		SdbListIter *iter;
		SdbKv *kv;
		int j = 0;
		ls_foreach (sls, iter, kv) {
			int len = strlen (buf->data + chr);
			int minlen = R_MIN (len,  strlen (sdbkv_key (kv)));
			if (!len || !strncmp (buf->data + chr, sdbkv_key (kv), minlen)) {
				char *p = strchr (buf->data + chr, '.');
				if (p) {
					j += autocomplete_pfele (core, completion, sdbkv_key (kv), pfx, j, p + 1);
					break;
				} else {
					char *s = r_str_newf ("pf%s.%s", pfx, sdbkv_key (kv));
					r_line_completion_push (completion, s);
					free (s);
				}
			}
		}
	} else if ((!strncmp (buf->data, "afvn ", 5))
	|| (!strncmp (buf->data, "afan ", 5))) {
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
		RList *vars;
		if (!strncmp (buf->data, "afvn ", 5)) {
			vars = r_anal_var_list (core->anal, fcn, R_ANAL_VAR_KIND_BPV);
		} else {
			vars = r_anal_var_list (core->anal, fcn, R_ANAL_VAR_KIND_ARG);
		}
		const char *f_ptr, *l_ptr;
		RAnalVar *var;
		int len = strlen (buf->data);

		f_ptr = r_sub_str_lchr (buf->data, 0, buf->index, ' ');
		f_ptr = f_ptr != NULL ? f_ptr + 1 : buf->data;
		l_ptr = r_sub_str_rchr (buf->data, buf->index, len, ' ');
		if (!l_ptr) {
			l_ptr = buf->data + len;
		}
		RListIter *iter;
		r_list_foreach (vars, iter, var) {
			if (!strncmp (f_ptr, var->name, l_ptr - f_ptr)) {
				r_line_completion_push (completion, var->name);
			}
		}
		r_list_free (vars);
	} else if (!strncmp (buf->data, "t ", 2)
	|| !strncmp (buf->data, "t- ", 3)) {
		SdbList *l = sdb_foreach_list (core->anal->sdb_types, true);
		SdbListIter *iter;
		SdbKv *kv;
		int chr = (buf->data[1] == ' ')? 2: 3;
		ls_foreach (l, iter, kv) {
			int len = strlen (buf->data + chr);
			if (!len || !strncmp (buf->data + chr, sdbkv_key (kv), len)) {
				if (!strcmp (sdbkv_value (kv), "type") || !strcmp (sdbkv_value (kv), "enum")
				|| !strcmp (sdbkv_value (kv), "struct")) {
					r_line_completion_push (completion, sdbkv_key (kv));
				}
			}
		}
		ls_free (l);
	} else if ((!strncmp (buf->data, "te ", 3))) {
		SdbList *l = sdb_foreach_list (core->anal->sdb_types, true);
		SdbListIter *iter;
		SdbKv *kv;
		int chr = 3;
		ls_foreach (l, iter, kv) {
			int len = strlen (buf->data + chr);
			if (!len || !strncmp (buf->data + chr, sdbkv_key (kv), len)) {
				if (!strcmp (sdbkv_value (kv), "enum")) {
					r_line_completion_push (completion, sdbkv_key (kv));
				}
			}
		}
		ls_free (l);
	} else if (!strncmp (buf->data, "$", 1)) {
		int i;
		for (i = 0; i < core->rcmd->aliases.count; i++) {
			const char *key = core->rcmd->aliases.keys[i];
			int len = strlen (buf->data);
			if (!len || !strncmp (buf->data, key, len)) {
				r_line_completion_push (completion, key);
			}
		}
	} else if (!strncmp (buf->data, "ts ", 3)
	|| !strncmp (buf->data, "ta ", 3)
	|| !strncmp (buf->data, "tp ", 3)
	|| !strncmp (buf->data, "tl ", 3)
	|| !strncmp (buf->data, "tpx ", 4)
	|| !strncmp (buf->data, "tss ", 4)
	|| !strncmp (buf->data, "ts* ", 4)) {
		SdbList *l = sdb_foreach_list (core->anal->sdb_types, true);
		SdbListIter *iter;
		SdbKv *kv;
		int chr = (buf->data[2] == ' ')? 3: 4;
		ls_foreach (l, iter, kv) {
			int len = strlen (buf->data + chr);
			const char *key = sdbkv_key (kv);
			if (!len || !strncmp (buf->data + chr, key, len)) {
				if (!strncmp (sdbkv_value (kv), "struct", strlen ("struct") + 1)) {
					r_line_completion_push (completion, key);
				}
			}
		}
		ls_free (l);
	} else if (!strncmp (buf->data, "zo ", 3)
	|| !strncmp (buf->data, "zoz ", 4)) {
		if (core->anal->zign_path && core->anal->zign_path[0]) {
			char *zignpath = r_file_abspath (core->anal->zign_path);
			char *paths[2] = { zignpath, NULL };
			autocompleteFilename (completion, buf, paths, 1);
			free (zignpath);
		} else {
			autocompleteFilename (completion, buf, NULL, 1);
		}
	} else if (find_e_opts (core, completion, buf)) {
		return;
	} else if (prompt_type == R_LINE_PROMPT_OFFSET) {
		autocomplete_flags (core, completion, buf->data);
	} else if (prompt_type == R_LINE_PROMPT_FILE) {
		autocomplete_file (completion, buf->data);
	} else if (!find_autocomplete (core, completion, buf)) {
		autocomplete_default (core, completion, buf);
	}
}