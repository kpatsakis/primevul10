TIFFWriteRawTile(TIFF* tif, uint32 tile, void* data, tmsize_t cc)
{
	static const char module[] = "TIFFWriteRawTile";

	if (!WRITECHECKTILES(tif, module))
		return ((tmsize_t)(-1));
	if (tile >= tif->tif_dir.td_nstrips) {
		TIFFErrorExt(tif->tif_clientdata, module, "Tile %lu out of range, max %lu",
		    (unsigned long) tile,
		    (unsigned long) tif->tif_dir.td_nstrips);
		return ((tmsize_t)(-1));
	}
	return (TIFFAppendToStrip(tif, tile, (uint8*) data, cc) ?
	    cc : (tmsize_t)(-1));
}