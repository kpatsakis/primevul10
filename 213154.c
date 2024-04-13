static void set_prompt (RCore *r) {
	char tmp[128];
	char *filename = strdup ("");
	const char *cmdprompt = r_config_get (r->config, "cmd.prompt");
	const char *BEGIN = "";
	const char *END = "";
	const char *remote = "";

	if (cmdprompt && *cmdprompt) {
		r_core_cmd (r, cmdprompt, 0);
	}

	if (r_config_get_i (r->config, "scr.prompt.file")) {
		free (filename);
		filename = r_str_newf ("\"%s\"",
			r->io->desc ? r_file_basename (r->io->desc->name) : "");
	}
	if (r->cmdremote) {
		char *s = r_core_cmd_str (r, "s");
		r->offset = r_num_math (NULL, s);
		free (s);
		remote = "=!";
	}

	if (r_config_get_i (r->config, "scr.color")) {
		BEGIN = r->cons->context->pal.prompt;
		END = r->cons->context->pal.reset;
	}

	// TODO: also in visual prompt and disasm/hexdump ?
	if (r_config_get_i (r->config, "asm.segoff")) {
		ut32 a, b;
		unsigned int seggrn = r_config_get_i (r->config, "asm.seggrn");

		a = ((r->offset >> 16) << (16 - seggrn));
		b = (r->offset & 0xffff);
		snprintf (tmp, 128, "%04x:%04x", a, b);
	} else {
		char p[64], sec[32];
		int promptset = false;

		sec[0] = '\0';
		if (r_config_get_i (r->config, "scr.prompt.flag")) {
			promptset = prompt_flag (r, p, sizeof (p));
		}
		if (r_config_get_i (r->config, "scr.prompt.sect")) {
			prompt_sec (r, sec, sizeof (sec));
		}

		if (!promptset) {
			if (r->print->wide_offsets && r->dbg->bits & R_SYS_BITS_64) {
				snprintf (p, sizeof (p), "0x%016" PFMT64x, r->offset);
			} else {
				snprintf (p, sizeof (p), "0x%08" PFMT64x, r->offset);
			}
		}
		snprintf (tmp, sizeof (tmp), "%s%s", sec, p);
	}

	chop_prompt (filename, tmp, 128);
	char *prompt = r_str_newf ("%s%s[%s%s]>%s ", filename, BEGIN, remote,
		tmp, END);
	r_line_set_prompt (prompt ? prompt : "");

	R_FREE (filename);
	R_FREE (prompt);
}