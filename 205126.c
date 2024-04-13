TIFFGrowStrips(TIFF* tif, uint32 delta, const char* module)
{
	TIFFDirectory *td = &tif->tif_dir;
	uint64* new_stripoffset;
	uint64* new_stripbytecount;

	assert(td->td_planarconfig == PLANARCONFIG_CONTIG);
	new_stripoffset = (uint64*)_TIFFrealloc(td->td_stripoffset,
		(td->td_nstrips + delta) * sizeof (uint64));
	new_stripbytecount = (uint64*)_TIFFrealloc(td->td_stripbytecount,
		(td->td_nstrips + delta) * sizeof (uint64));
	if (new_stripoffset == NULL || new_stripbytecount == NULL) {
		if (new_stripoffset)
			_TIFFfree(new_stripoffset);
		if (new_stripbytecount)
			_TIFFfree(new_stripbytecount);
		td->td_nstrips = 0;
		TIFFErrorExt(tif->tif_clientdata, module, "No space to expand strip arrays");
		return (0);
	}
	td->td_stripoffset = new_stripoffset;
	td->td_stripbytecount = new_stripbytecount;
	_TIFFmemset(td->td_stripoffset + td->td_nstrips,
		    0, delta*sizeof (uint64));
	_TIFFmemset(td->td_stripbytecount + td->td_nstrips,
		    0, delta*sizeof (uint64));
	td->td_nstrips += delta;
        tif->tif_flags |= TIFF_DIRTYDIRECT;

	return (1);
}