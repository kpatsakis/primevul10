R_API RList *r_str_split_list(char *str, const char *c, int n)  {
	r_return_val_if_fail (str && c, NULL);
	RList *lst = r_list_newf (NULL);
	char *aux = str;
	int i = 0;
	char  *e = aux;
	for (;e;) {
		e = strstr (aux, c);
		if (n > 0) {
			if (++i > n) {
				r_list_append (lst, aux);
				break;
			}
		}
		if (e) {
			*e++ =  0;
		}
		r_str_trim (aux);
		r_list_append (lst, aux);
		aux = e;
	}
	return lst;
}