R_API RList *r_str_split_duplist(const char *_str, const char *c) {
	r_return_val_if_fail (_str && c, NULL);
	RList *lst = r_list_newf (free);
	char *str = strdup (_str);
	char *aux = str;
	size_t clen = strlen (c);
	while (aux) {
		char *next = strstr (aux, c);
		if (next) {
			*next = '\0';
			next += clen;
		}
		r_str_trim (aux);
		r_list_append (lst, strdup (aux));
		aux = next;
	}
	free (str);
	return lst;
}