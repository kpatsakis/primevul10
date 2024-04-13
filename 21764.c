http_GetHdrQ(const struct http *hp, hdr_t hdr, const char *field)
{
	const char *hb, *he, *b, *e;
	int i;
	double a, f;

	i = http_GetHdrToken(hp, hdr, field, &hb, &he);
	if (!i)
		return (0.);

	if (hb == NULL)
		return (1.);
	while (http_split(&hb, he, ";", &b, &e)) {
		if (*b != 'q')
			continue;
		for (b++; b < e && vct_issp(*b); b++)
			continue;
		if (b == e || *b != '=')
			continue;
		break;
	}
	if (b == NULL)
		return (1.);
	for (b++; b < e && vct_issp(*b); b++)
		continue;
	if (b == e || (*b != '.' && !vct_isdigit(*b)))
		return (0.);
	a = 0;
	while (b < e && vct_isdigit(*b)) {
		a *= 10.;
		a += *b - '0';
		b++;
	}
	if (b == e || *b++ != '.')
		return (a);
	f = .1;
	while (b < e && vct_isdigit(*b)) {
		a += f * (*b - '0');
		f *= .1;
		b++;
	}
	return (a);
}