DECLAREwriteFunc(writeBufferToSeparateStrips)
{
	uint32 rowsize = imagewidth * spp;
	uint32 rowsperstrip;
	tsize_t stripsize = TIFFStripSize(out);
	tdata_t obuf;
	tstrip_t strip = 0;
	tsample_t s;

	obuf = _TIFFmalloc(stripsize);
	if (obuf == NULL)
		return (0);
	_TIFFmemset(obuf, 0, stripsize);
	(void) TIFFGetFieldDefaulted(out, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
	for (s = 0; s < spp; s++) {
		uint32 row;
		for (row = 0; row < imagelength; row += rowsperstrip) {
			uint32 nrows = (row+rowsperstrip > imagelength) ?
			    imagelength-row : rowsperstrip;
			tsize_t stripsize = TIFFVStripSize(out, nrows);

			cpContigBufToSeparateBuf(
			    obuf, (uint8*) buf + row*rowsize + s,
			    nrows, imagewidth, 0, 0, spp, 1);
			if (TIFFWriteEncodedStrip(out, strip++, obuf, stripsize) < 0) {
				TIFFError(TIFFFileName(out),
				    "Error, can't write strip %u",
				    strip - 1);
				_TIFFfree(obuf);
				return 0;
			}
		}
	}
	_TIFFfree(obuf);
	return 1;

}