http_VSL_log(const struct http *hp)
{
	unsigned u;

	for (u = 0; u < hp->nhd; u++)
		if (hp->hd[u].b != NULL)
			http_VSLH(hp, u);
}