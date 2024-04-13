void free_cache_bitmap_order(rdpContext* context, CACHE_BITMAP_ORDER* order)
{
	if (order)
		free(order->bitmapDataStream);

	free(order);
}