static BOOL update_gdi_mem3blt(rdpContext* context, MEM3BLT_ORDER* mem3blt)
{
	BYTE style;
	rdpBitmap* bitmap;
	rdpCache* cache = context->cache;
	rdpBrush* brush = &mem3blt->brush;
	BOOL ret = TRUE;

	if (mem3blt->cacheId == 0xFF)
		bitmap = offscreen_cache_get(cache->offscreen, mem3blt->cacheIndex);
	else
		bitmap = bitmap_cache_get(cache->bitmap, (BYTE)mem3blt->cacheId, mem3blt->cacheIndex);

	/* XP-SP2 servers sometimes ask for cached bitmaps they've never defined. */
	if (!bitmap)
		return TRUE;

	style = brush->style;

	if (brush->style & CACHED_BRUSH)
	{
		brush->data = brush_cache_get(cache->brush, brush->index, &brush->bpp);

		if (!brush->data)
			return FALSE;

		brush->style = 0x03;
	}

	mem3blt->bitmap = bitmap;
	IFCALLRET(cache->bitmap->Mem3Blt, ret, context, mem3blt);
	brush->style = style;
	return ret;
}