CACHE_BITMAP_ORDER* copy_cache_bitmap_order(rdpContext* context, const CACHE_BITMAP_ORDER* order)
{
	CACHE_BITMAP_ORDER* dst = calloc(1, sizeof(CACHE_BITMAP_ORDER));

	if (!dst || !order)
		goto fail;

	*dst = *order;

	if (order->bitmapLength > 0)
	{
		dst->bitmapDataStream = malloc(order->bitmapLength);

		if (!dst->bitmapDataStream)
			goto fail;

		memcpy(dst->bitmapDataStream, order->bitmapDataStream, order->bitmapLength);
	}

	return dst;
fail:
	free_cache_bitmap_order(context, dst);
	return NULL;
}