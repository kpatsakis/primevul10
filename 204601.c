BuildMapUaToAa(TIFFRGBAImage* img)
{
	static const char module[]="BuildMapUaToAa";
	uint8* m;
	uint16 na,nv;
	assert(img->UaToAa==NULL);
	img->UaToAa=_TIFFmalloc(65536);
	if (img->UaToAa==NULL)
	{
		TIFFErrorExt(img->tif->tif_clientdata,module,"Out of memory");
		return(0);
	}
	m=img->UaToAa;
	for (na=0; na<256; na++)
	{
		for (nv=0; nv<256; nv++)
			*m++=(nv*na+127)/255;
	}
	return(1);
}