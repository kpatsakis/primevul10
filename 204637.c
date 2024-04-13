TIFFReadRGBAImageOriented(TIFF* tif,
			  uint32 rwidth, uint32 rheight, uint32* raster,
			  int orientation, int stop)
{
    char emsg[1024] = "";
    TIFFRGBAImage img;
    int ok;

	if (TIFFRGBAImageOK(tif, emsg) && TIFFRGBAImageBegin(&img, tif, stop, emsg)) {
		img.req_orientation = orientation;
		/* XXX verify rwidth and rheight against width and height */
		ok = TIFFRGBAImageGet(&img, raster+(rheight-img.height)*rwidth,
			rwidth, img.height);
		TIFFRGBAImageEnd(&img);
	} else {
		TIFFErrorExt(tif->tif_clientdata, TIFFFileName(tif), "%s", emsg);
		ok = 0;
    }
    return (ok);
}