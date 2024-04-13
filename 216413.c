void free_cache_bitmap_v2_order(rdpContext* context, CACHE_BITMAP_V2_ORDER* order)
{
	if (order)
		free(order->bitmapDataStream);

	free(order);
}