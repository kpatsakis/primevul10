gtTileSeparate(TIFFRGBAImage* img, uint32* raster, uint32 w, uint32 h)
{
	TIFF* tif = img->tif;
	tileSeparateRoutine put = img->put.separate;
	uint32 col, row, y, rowstoread;
	tmsize_t pos;
	uint32 tw, th;
	unsigned char* buf;
	unsigned char* p0;
	unsigned char* p1;
	unsigned char* p2;
	unsigned char* pa;
	tmsize_t tilesize;
	tmsize_t bufsize;
	int32 fromskew, toskew;
	int alpha = img->alpha;
	uint32 nrow;
	int ret = 1, flip;
        int colorchannels;

	tilesize = TIFFTileSize(tif);  
	bufsize = TIFFSafeMultiply(tmsize_t,alpha?4:3,tilesize);
	if (bufsize == 0) {
		TIFFErrorExt(tif->tif_clientdata, TIFFFileName(tif), "Integer overflow in %s", "gtTileSeparate");
		return (0);
	}
	buf = (unsigned char*) _TIFFmalloc(bufsize);
	if (buf == 0) {
		TIFFErrorExt(tif->tif_clientdata, TIFFFileName(tif), "%s", "No space for tile buffer");
		return (0);
	}
	_TIFFmemset(buf, 0, bufsize);
	p0 = buf;
	p1 = p0 + tilesize;
	p2 = p1 + tilesize;
	pa = (alpha?(p2+tilesize):NULL);
	TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
	TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);

	flip = setorientation(img);
	if (flip & FLIP_VERTICALLY) {
		y = h - 1;
		toskew = -(int32)(tw + w);
	}
	else {
		y = 0;
		toskew = -(int32)(tw - w);
	}

        switch( img->photometric )
        {
          case PHOTOMETRIC_MINISWHITE:
          case PHOTOMETRIC_MINISBLACK:
          case PHOTOMETRIC_PALETTE:
            colorchannels = 1;
            p2 = p1 = p0;
            break;

          default:
            colorchannels = 3;
            break;
        }

	for (row = 0; row < h; row += nrow)
	{
		rowstoread = th - (row + img->row_offset) % th;
		nrow = (row + rowstoread > h ? h - row : rowstoread);
		for (col = 0; col < w; col += tw)
		{
			if (TIFFReadTile(tif, p0, col+img->col_offset,  
			    row+img->row_offset,0,0)==(tmsize_t)(-1) && img->stoponerr)
			{
				ret = 0;
				break;
			}
			if (colorchannels > 1 
                            && TIFFReadTile(tif, p1, col+img->col_offset,  
                                            row+img->row_offset,0,1) == (tmsize_t)(-1) 
                            && img->stoponerr)
			{
				ret = 0;
				break;
			}
			if (colorchannels > 1 
                            && TIFFReadTile(tif, p2, col+img->col_offset,  
                                            row+img->row_offset,0,2) == (tmsize_t)(-1) 
                            && img->stoponerr)
			{
				ret = 0;
				break;
			}
			if (alpha
                            && TIFFReadTile(tif,pa,col+img->col_offset,  
                                            row+img->row_offset,0,colorchannels) == (tmsize_t)(-1) 
                            && img->stoponerr)
                        {
                            ret = 0;
                            break;
			}

			pos = ((row+img->row_offset) % th) * TIFFTileRowSize(tif);  

			if (col + tw > w)
			{
				/*
				 * Tile is clipped horizontally.  Calculate
				 * visible portion and skewing factors.
				 */
				uint32 npix = w - col;
				fromskew = tw - npix;
				(*put)(img, raster+y*w+col, col, y,
				    npix, nrow, fromskew, toskew + fromskew,
				    p0 + pos, p1 + pos, p2 + pos, (alpha?(pa+pos):NULL));
			} else {
				(*put)(img, raster+y*w+col, col, y,
				    tw, nrow, 0, toskew, p0 + pos, p1 + pos, p2 + pos, (alpha?(pa+pos):NULL));
			}
		}

		y += (flip & FLIP_VERTICALLY ?-(int32) nrow : (int32) nrow);
	}

	if (flip & FLIP_HORIZONTALLY) {
		uint32 line;

		for (line = 0; line < h; line++) {
			uint32 *left = raster + (line * w);
			uint32 *right = left + w - 1;

			while ( left < right ) {
				uint32 temp = *left;
				*left = *right;
				*right = temp;
				left++, right--;
			}
		}
	}

	_TIFFfree(buf);
	return (ret);
}