static BOOL update_gdi_memblt(rdpContext* context, MEMBLT_ORDER* memblt)
{
	rdpBitmap* bitmap;
	rdpCache* cache = context->cache;

	if (memblt->cacheId == 0xFF)
		bitmap = offscreen_cache_get(cache->offscreen, memblt->cacheIndex);
	else
		bitmap = bitmap_cache_get(cache->bitmap, (BYTE)memblt->cacheId, memblt->cacheIndex);

	/* XP-SP2 servers sometimes ask for cached bitmaps they've never defined. */
	if (bitmap == NULL)
		return TRUE;

	memblt->bitmap = bitmap;
	return IFCALLRESULT(TRUE, cache->bitmap->MemBlt, context, memblt);
}