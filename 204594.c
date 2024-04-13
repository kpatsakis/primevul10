BuildMapBitdepth16To8(TIFFRGBAImage* img)
{
	static const char module[]="BuildMapBitdepth16To8";
	uint8* m;
	uint32 n;
	assert(img->Bitdepth16To8==NULL);
	img->Bitdepth16To8=_TIFFmalloc(65536);
	if (img->Bitdepth16To8==NULL)
	{
		TIFFErrorExt(img->tif->tif_clientdata,module,"Out of memory");
		return(0);
	}
	m=img->Bitdepth16To8;
	for (n=0; n<65536; n++)
		*m++=(n+128)/257;
	return(1);
}