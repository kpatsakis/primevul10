static BOOL update_gdi_cache_bitmap(rdpContext* context, const CACHE_BITMAP_ORDER* cacheBitmap)
{
	rdpBitmap* bitmap;
	rdpBitmap* prevBitmap;
	rdpCache* cache = context->cache;
	bitmap = Bitmap_Alloc(context);

	if (!bitmap)
		return FALSE;

	Bitmap_SetDimensions(bitmap, cacheBitmap->bitmapWidth, cacheBitmap->bitmapHeight);

	if (!bitmap->Decompress(context, bitmap, cacheBitmap->bitmapDataStream,
	                        cacheBitmap->bitmapWidth, cacheBitmap->bitmapHeight,
	                        cacheBitmap->bitmapBpp, cacheBitmap->bitmapLength,
	                        cacheBitmap->compressed, RDP_CODEC_ID_NONE))
	{
		Bitmap_Free(context, bitmap);
		return FALSE;
	}

	if (!bitmap->New(context, bitmap))
	{
		Bitmap_Free(context, bitmap);
		return FALSE;
	}

	prevBitmap = bitmap_cache_get(cache->bitmap, cacheBitmap->cacheId, cacheBitmap->cacheIndex);
	Bitmap_Free(context, prevBitmap);
	return bitmap_cache_put(cache->bitmap, cacheBitmap->cacheId, cacheBitmap->cacheIndex, bitmap);
}