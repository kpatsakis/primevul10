TIFFRGBAImageEnd(TIFFRGBAImage* img)
{
	if (img->Map)
		_TIFFfree(img->Map), img->Map = NULL;
	if (img->BWmap)
		_TIFFfree(img->BWmap), img->BWmap = NULL;
	if (img->PALmap)
		_TIFFfree(img->PALmap), img->PALmap = NULL;
	if (img->ycbcr)
		_TIFFfree(img->ycbcr), img->ycbcr = NULL;
	if (img->cielab)
		_TIFFfree(img->cielab), img->cielab = NULL;
	if (img->UaToAa)
		_TIFFfree(img->UaToAa), img->UaToAa = NULL;
	if (img->Bitdepth16To8)
		_TIFFfree(img->Bitdepth16To8), img->Bitdepth16To8 = NULL;

	if( img->redcmap ) {
		_TIFFfree( img->redcmap );
		_TIFFfree( img->greencmap );
		_TIFFfree( img->bluecmap );
                img->redcmap = img->greencmap = img->bluecmap = NULL;
	}
}