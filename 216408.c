static BOOL update_gdi_cache_bitmap_v3(rdpContext* context, CACHE_BITMAP_V3_ORDER* cacheBitmapV3)
{
	rdpBitmap* bitmap;
	rdpBitmap* prevBitmap;
	BOOL compressed = TRUE;
	rdpCache* cache = context->cache;
	rdpSettings* settings = context->settings;
	BITMAP_DATA_EX* bitmapData = &cacheBitmapV3->bitmapData;
	bitmap = Bitmap_Alloc(context);

	if (!bitmap)
		return FALSE;

	if (!cacheBitmapV3->bpp)
		cacheBitmapV3->bpp = settings->ColorDepth;

	compressed = (bitmapData->codecID != RDP_CODEC_ID_NONE);
	Bitmap_SetDimensions(bitmap, bitmapData->width, bitmapData->height);

	if (!bitmap->Decompress(context, bitmap, bitmapData->data, bitmapData->width,
	                        bitmapData->height, bitmapData->bpp, bitmapData->length, compressed,
	                        bitmapData->codecID))
	{
		Bitmap_Free(context, bitmap);
		return FALSE;
	}

	if (!bitmap->New(context, bitmap))
	{
		Bitmap_Free(context, bitmap);
		return FALSE;
	}

	prevBitmap = bitmap_cache_get(cache->bitmap, cacheBitmapV3->cacheId, cacheBitmapV3->cacheIndex);
	Bitmap_Free(context, prevBitmap);
	return bitmap_cache_put(cache->bitmap, cacheBitmapV3->cacheId, cacheBitmapV3->cacheIndex,
	                        bitmap);
}