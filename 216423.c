static BOOL update_gdi_cache_bitmap_v2(rdpContext* context, CACHE_BITMAP_V2_ORDER* cacheBitmapV2)

{
	rdpBitmap* bitmap;
	rdpBitmap* prevBitmap;
	rdpCache* cache = context->cache;
	rdpSettings* settings = context->settings;
	bitmap = Bitmap_Alloc(context);

	if (!bitmap)
		return FALSE;

	if (!cacheBitmapV2->bitmapBpp)
		cacheBitmapV2->bitmapBpp = settings->ColorDepth;

	if ((settings->ColorDepth == 15) && (cacheBitmapV2->bitmapBpp == 16))
		cacheBitmapV2->bitmapBpp = settings->ColorDepth;

	Bitmap_SetDimensions(bitmap, cacheBitmapV2->bitmapWidth, cacheBitmapV2->bitmapHeight);

	if (!bitmap->Decompress(context, bitmap, cacheBitmapV2->bitmapDataStream,
	                        cacheBitmapV2->bitmapWidth, cacheBitmapV2->bitmapHeight,
	                        cacheBitmapV2->bitmapBpp, cacheBitmapV2->bitmapLength,
	                        cacheBitmapV2->compressed, RDP_CODEC_ID_NONE))
	{
		Bitmap_Free(context, bitmap);
		return FALSE;
	}

	prevBitmap = bitmap_cache_get(cache->bitmap, cacheBitmapV2->cacheId, cacheBitmapV2->cacheIndex);

	if (!bitmap->New(context, bitmap))
	{
		Bitmap_Free(context, bitmap);
		return FALSE;
	}

	Bitmap_Free(context, prevBitmap);
	return bitmap_cache_put(cache->bitmap, cacheBitmapV2->cacheId, cacheBitmapV2->cacheIndex,
	                        bitmap);
}