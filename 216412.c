void free_cache_bitmap_v3_order(rdpContext* context, CACHE_BITMAP_V3_ORDER* order)
{
	if (order)
		free(order->bitmapData.data);

	free(order);
}