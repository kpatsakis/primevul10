gtTileContig(TIFFRGBAImage* img, uint32* raster, uint32 w, uint32 h)
{
    TIFF* tif = img->tif;
    tileContigRoutine put = img->put.contig;
    uint32 col, row, y, rowstoread;
    tmsize_t pos;
    uint32 tw, th;
    unsigned char* buf;
    int32 fromskew, toskew;
    uint32 nrow;
    int ret = 1, flip;

    buf = (unsigned char*) _TIFFmalloc(TIFFTileSize(tif));
    if (buf == 0) {
		TIFFErrorExt(tif->tif_clientdata, TIFFFileName(tif), "%s", "No space for tile buffer");
		return (0);
    }
    _TIFFmemset(buf, 0, TIFFTileSize(tif));
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
     
    for (row = 0; row < h; row += nrow)
    {
        rowstoread = th - (row + img->row_offset) % th;
    	nrow = (row + rowstoread > h ? h - row : rowstoread);
	for (col = 0; col < w; col += tw) 
        {
	    if (TIFFReadTile(tif, buf, col+img->col_offset,  
			     row+img->row_offset, 0, 0)==(tmsize_t)(-1) && img->stoponerr)
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
                       npix, nrow, fromskew, toskew + fromskew, buf + pos);
            }
            else 
            {
                (*put)(img, raster+y*w+col, col, y, tw, nrow, 0, toskew, buf + pos);
            }
        }

        y += (flip & FLIP_VERTICALLY ? -(int32) nrow : (int32) nrow);
    }
    _TIFFfree(buf);

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

    return (ret);
}