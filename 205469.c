JPEGFixupTags(TIFF* tif)
{
#ifdef CHECK_JPEG_YCBCR_SUBSAMPLING
	if ((tif->tif_dir.td_photometric==PHOTOMETRIC_YCBCR)&&
	    (tif->tif_dir.td_planarconfig==PLANARCONFIG_CONTIG)&&
	    (tif->tif_dir.td_samplesperpixel==3))
		JPEGFixupTagsSubsampling(tif);
#endif
        
	return(1);
}