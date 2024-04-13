R_API char *r_core_editor(const RCore *core, const char *file, const char *str) {
	const bool interactive = r_cons_is_interactive ();
	const char *editor = r_config_get (core->config, "cfg.editor");
	char *name = NULL, *ret = NULL;
	int fd;

	if (!interactive || !editor || !*editor) {
		return NULL;
	}
	bool readonly = false;
	if (file && *file != '*') {
		name = strdup (file);
		fd = r_sandbox_open (file, O_RDWR, 0644);
		if (fd == -1) {
			fd = r_sandbox_open (file, O_RDONLY, 0644);
			readonly = true;
		}
	} else {
		fd = r_file_mkstemp (file, &name);
	}
	if (fd == -1) {
		free (name);
		return NULL;
	}
	if (readonly) {
		eprintf ("Opening in read-only\n");
	} else {
		if (str) {
			write (fd, str, strlen (str));
		}
	}
	close (fd);

	if (name && (!editor || !*editor || !strcmp (editor, "-"))) {
		RCons *cons = r_cons_singleton ();
		void *tmp = cons->cb_editor;
		cons->cb_editor = NULL;
		r_cons_editor (name, NULL);
		cons->cb_editor = tmp;
	} else {
		if (editor && name) {
			r_sys_cmdf ("%s '%s'", editor, name);
		}
	}
	size_t len = 0;
	ret = name? r_file_slurp (name, &len): 0;
	if (ret) {
		if (len && ret[len - 1] == '\n') {
			ret[len - 1] = 0; // chop
		}
		if (!file) {
			r_file_rm (name);
		}
	}
	free (name);
	return ret;
}