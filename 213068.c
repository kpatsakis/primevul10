static void autocomplete_process_path(RLineCompletion *completion, const char *str, const char *path) {
	char *lpath = NULL, *dirname = NULL , *basename = NULL;
	char *home = NULL, *filename = NULL, *p = NULL;
	int n = 0;
	RList *list;
	RListIter *iter;

	if (!path) {
		goto out;
	}

	lpath = r_str_new (path);
#if __WINDOWS__
	r_str_replace_ch (lpath, '/', '\\', true);
#endif
	p = (char *)r_str_last (lpath, R_SYS_DIR);
	if (p) {
		*p = 0;
		if (p == lpath) { // /xxx
#if __WINDOWS__
			dirname = strdup ("\\.\\");
#else
			dirname = r_str_new (R_SYS_DIR);
#endif
		} else if (lpath[0] == '~' && lpath[1]) { // ~/xxx/yyy
			dirname = r_str_home (lpath + 2);
		} else if (lpath[0] == '~') { // ~/xxx
			if (!(home = r_str_home (NULL))) {
				goto out;
			}
			dirname = r_str_newf ("%s%s", home, R_SYS_DIR);
			free (home);
		} else if (lpath[0] == '.' || lpath[0] == R_SYS_DIR[0] ) { // ./xxx/yyy || /xxx/yyy
			dirname = r_str_newf ("%s%s", lpath, R_SYS_DIR);
		} else { // xxx/yyy
			char *fmt = ".%s%s%s";
#if __WINDOWS__
			if (strchr (path, ':')) {
				fmt = "%.0s%s%s";
			}
#endif
			dirname = r_str_newf (fmt, R_SYS_DIR, lpath, R_SYS_DIR);
		}
		basename = r_str_new (p + 1);
	} else { // xxx
		dirname = r_str_newf (".%s", R_SYS_DIR);
		basename = r_str_new (lpath);
	}

	if (!dirname || !basename) {
		goto out;
	}

	list= r_sys_dir (dirname);
	n = strlen (basename);
	bool chgdir = !strncmp (str, "cd ", 3);
	if (list) {
		r_list_foreach (list, iter, filename) {
			if (*filename == '.') {
				continue;
			}
			if (!basename[0] || !strncmp (filename, basename, n))  {
				char *tmpstring = r_str_newf ("%s%s", dirname, filename);
				if (r_file_is_directory (tmpstring)) {
					char *s = r_str_newf ("%s%s", tmpstring, R_SYS_DIR);
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
out:
	free (lpath);
	free (dirname);
	free (basename);
}