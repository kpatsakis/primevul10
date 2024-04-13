GF_Err gf_isom_remove_root_od(GF_ISOFile *movie)
{
	GF_Err e;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	if (!movie->moov || !movie->moov->iods) return GF_OK;
	gf_isom_box_del_parent(&movie->moov->child_boxes, (GF_Box *)movie->moov->iods);
	movie->moov->iods = NULL;
	return GF_OK;
}