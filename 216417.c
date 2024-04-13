CACHE_BITMAP_V3_ORDER* copy_cache_bitmap_v3_order(rdpContext* context,
                                                  const CACHE_BITMAP_V3_ORDER* order)
{
	CACHE_BITMAP_V3_ORDER* dst = calloc(1, sizeof(CACHE_BITMAP_V3_ORDER));

	if (!dst || !order)
		goto fail;

	*dst = *order;

	if (order->bitmapData.length > 0)
	{
		dst->bitmapData.data = malloc(order->bitmapData.length);

		if (!dst->bitmapData.data)
			goto fail;

		memcpy(dst->bitmapData.data, order->bitmapData.data, order->bitmapData.length);
	}

	return dst;
fail:
	free_cache_bitmap_v3_order(context, dst);
	return NULL;
}