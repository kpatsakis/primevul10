HTTP_Clone(struct http *to, const struct http * const fm)
{

	HTTP_Dup(to, fm);
	to->vsl = fm->vsl;
	to->ws = fm->ws;
}