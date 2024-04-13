http_Proto(struct http *to)
{
	const char *fm;

	fm = to->hd[HTTP_HDR_PROTO].b;

	if (fm != NULL &&
	    (fm[0] == 'H' || fm[0] == 'h') &&
	    (fm[1] == 'T' || fm[1] == 't') &&
	    (fm[2] == 'T' || fm[2] == 't') &&
	    (fm[3] == 'P' || fm[3] == 'p') &&
	    fm[4] == '/' &&
	    vct_isdigit(fm[5]) &&
	    fm[6] == '.' &&
	    vct_isdigit(fm[7]) &&
	    fm[8] == '\0') {
		to->protover = 10 * (fm[5] - '0') + (fm[7] - '0');
	} else {
		to->protover = 0;
	}
}