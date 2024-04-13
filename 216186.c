R_API char *r_str_scale(const char *s, int w, int h) {
	// count lines and rows in (s) string
	// compute how many lines we should remove or combine
	// return a string containing
	// for now this function is ascii only (no utf8 or ansi escapes)
	RListIter *iter;
	char *line;
	char *str = strdup (s);
	RList *lines = r_str_split_list (str, "\n", 0);
	int i, j;
	int rows = 0;
	int maxcol = 0;

	rows = r_list_length (lines);
	r_list_foreach (lines, iter, line) {
		maxcol = R_MAX (strlen (line), maxcol);
	}

	RList *out = r_list_newf (free);

	int curline = -1;
	char *linetext = (char*)r_str_pad (' ', w);
	for (i = 0; i < h; i++) {
		int zoomedline = i * ((float)rows / h);
		const char *srcline = r_list_get_n (lines, zoomedline);
		int cols = strlen (srcline);
		for (j = 0; j < w; j++) {
			int zoomedcol = j * ( (float)cols / w);
			linetext[j] = srcline[zoomedcol];
		}
		if (curline != zoomedline) {
			r_list_append (out, strdup (linetext));
			curline = zoomedline;
		}
		memset (linetext, ' ', w);
	}
	free (str);
	return r_str_list_join (out, "\n");
}