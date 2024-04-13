static void free_bitmap_data(BITMAP_DATA* data, size_t count)
{
	size_t x;

	if (!data)
		return;

	for (x = 0; x < count; x++)
		free(data[x].bitmapDataStream);

	free(data);
}