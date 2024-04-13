static void autocomplete_ms_path(RLineCompletion *completion, RCore *core, const char *str, const char *path) {
	char *lpath = NULL, *dirname = NULL , *basename = NULL;
	char *p = NULL;
	char *pwd = (core->rfs && *(core->rfs->cwd)) ? *(core->rfs->cwd): ".";
	int n = 0;
	RList *list;
	RListIter *iter;
	RFSFile *file;
	r_return_if_fail (path);
	lpath = r_str_new (path);
	p = (char *)r_str_last (lpath, R_SYS_DIR);
	if (p) {
		*p = 0;
		if (p == lpath) { // /xxx
			dirname  = r_str_new ("/");
		} else if (lpath[0] == '.') { // ./xxx/yyy
			dirname = r_str_newf ("%s%s", pwd, R_SYS_DIR);
		} else if (lpath[0] == '/') { // /xxx/yyy
      			dirname = r_str_newf ("%s%s", lpath, R_SYS_DIR);
    		} else { // xxx/yyy
      			if (strlen (pwd) == 1) { // if pwd is root
        			dirname = r_str_newf ("%s%s%s", R_SYS_DIR, lpath, R_SYS_DIR);
      			} else {
				dirname = r_str_newf ("%s%s%s%s", pwd, R_SYS_DIR, lpath, R_SYS_DIR);
      			}
		}
		basename = r_str_new (p + 1);
	} else { // xxx
    		if (strlen (pwd) == 1) {
      			dirname = r_str_newf ("%s", R_SYS_DIR);
    		} else {
      			dirname = r_str_newf ("%s%s", pwd, R_SYS_DIR);
    		}
		basename = r_str_new (lpath);
	}

	if (!dirname || !basename) {
		goto out;
	}
	list= r_fs_dir (core->fs, dirname);
	n = strlen (basename);
	bool chgdir = !strncmp (str, "cd  ", 3);
	if (list) {
		r_list_foreach (list, iter, file) {
			if (!file) {
				continue;
			}
			if (!basename[0] || !strncmp (file->name, basename, n))  {
				char *tmpstring = r_str_newf ("%s%s", dirname, file->name);
				if (r_file_is_directory (tmpstring)) {
					char *s = r_str_newf ("%s/", tmpstring);
					r_line_completion_push (completion, s);
					free (s);
				} else if (!chgdir) {
					r_line_completion_push (completion, tmpstring);
				}
				free (tmpstring);
			}
		}
		r_list_free (list);
	}
	autocomplete_mount_point (completion, core, path);
out:
	free (lpath);
	free (dirname);
	free (basename);
}