initYCbCrConversion(TIFFRGBAImage* img)
{
	static const char module[] = "initYCbCrConversion";

	float *luma, *refBlackWhite;

	if (img->ycbcr == NULL) {
		img->ycbcr = (TIFFYCbCrToRGB*) _TIFFmalloc(
		    TIFFroundup_32(sizeof (TIFFYCbCrToRGB), sizeof (long))  
		    + 4*256*sizeof (TIFFRGBValue)
		    + 2*256*sizeof (int)
		    + 3*256*sizeof (int32)
		    );
		if (img->ycbcr == NULL) {
			TIFFErrorExt(img->tif->tif_clientdata, module,
			    "No space for YCbCr->RGB conversion state");
			return (0);
		}
	}

	TIFFGetFieldDefaulted(img->tif, TIFFTAG_YCBCRCOEFFICIENTS, &luma);
	TIFFGetFieldDefaulted(img->tif, TIFFTAG_REFERENCEBLACKWHITE,
	    &refBlackWhite);
	if (TIFFYCbCrToRGBInit(img->ycbcr, luma, refBlackWhite) < 0)
		return(0);
	return (1);
}