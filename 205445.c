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
        uint16 colorchannels;
	uint32 this_tw, tocol;
	int32 this_toskew, leftmost_toskew;
	int32 leftmost_fromskew;
	uint32 leftmost_tw;

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

	/*
	 *	Leftmost tile is clipped on left side if col_offset > 0.
	 */
	leftmost_fromskew = img->col_offset % tw;
	leftmost_tw = tw - leftmost_fromskew;
	leftmost_toskew = toskew + leftmost_fromskew;
	for (row = 0; row < h; row += nrow)
	{
		rowstoread = th - (row + img->row_offset) % th;
		nrow = (row + rowstoread > h ? h - row : rowstoread);
		fromskew = leftmost_fromskew;
		this_tw = leftmost_tw;
		this_toskew = leftmost_toskew;
		tocol = 0;
		col = img->col_offset;
		while (tocol < w)
		{
			if (TIFFReadTile(tif, p0, col,  
			    row+img->row_offset,0,0)==(tmsize_t)(-1) && img->stoponerr)
			{
				ret = 0;
				break;
			}
			if (colorchannels > 1 
                            && TIFFReadTile(tif, p1, col,  
                                            row+img->row_offset,0,1) == (tmsize_t)(-1) 
                            && img->stoponerr)
			{
				ret = 0;
				break;
			}
			if (colorchannels > 1 
                            && TIFFReadTile(tif, p2, col,  
                                            row+img->row_offset,0,2) == (tmsize_t)(-1) 
                            && img->stoponerr)
			{
				ret = 0;
				break;
			}
			if (alpha
                            && TIFFReadTile(tif,pa,col,  
                                            row+img->row_offset,0,colorchannels) == (tmsize_t)(-1) 
                            && img->stoponerr)
                        {
                            ret = 0;
                            break;
			}

			pos = ((row+img->row_offset) % th) * TIFFTileRowSize(tif) + \
			   ((tmsize_t) fromskew * img->samplesperpixel);
			if (tocol + this_tw > w) 
			{
				/*
				 * Rightmost tile is clipped on right side.
				 */
				fromskew = tw - (w - tocol);
				this_tw = tw - fromskew;
				this_toskew = toskew + fromskew;
			}
			(*put)(img, raster+y*w+tocol, tocol, y, this_tw, nrow, fromskew, this_toskew, \
				p0 + pos, p1 + pos, p2 + pos, (alpha?(pa+pos):NULL));
			tocol += this_tw;
			col += this_tw;
			/*
			* After the leftmost tile, tiles are no longer clipped on left side.
			*/
			fromskew = 0;
			this_tw = tw;
			this_toskew = toskew;
		}

		y += ((flip & FLIP_VERTICALLY) ?-(int32) nrow : (int32) nrow);
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
				left++;
				right--;
			}
		}
	}

	_TIFFfree(buf);
	return (ret);
}