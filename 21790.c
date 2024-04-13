HTTP_Dup(struct http *to, const struct http * const fm)
{

	assert(fm->nhd <= to->shd);
	memcpy(to->hd, fm->hd, fm->nhd * sizeof *to->hd);
	memcpy(to->hdf, fm->hdf, fm->nhd * sizeof *to->hdf);
	to->nhd = fm->nhd;
	to->logtag = fm->logtag;
	to->status = fm->status;
	to->protover = fm->protover;
}