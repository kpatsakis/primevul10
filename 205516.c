TIFFjpeg_tables_src(JPEGState* sp, TIFF* tif)
{
	TIFFjpeg_data_src(sp, tif);
	sp->src.init_source = tables_init_source;
}