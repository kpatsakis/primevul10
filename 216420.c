static BITMAP_DATA* copy_bitmap_data(const BITMAP_DATA* data, size_t count)
{
	size_t x;
	BITMAP_DATA* dst = (BITMAP_DATA*)calloc(count, sizeof(BITMAP_DATA));

	if (!dst)
		goto fail;

	for (x = 0; x < count; x++)
	{
		dst[x] = data[x];

		if (data[x].bitmapLength > 0)
		{
			dst[x].bitmapDataStream = malloc(data[x].bitmapLength);

			if (!dst[x].bitmapDataStream)
				goto fail;

			memcpy(dst[x].bitmapDataStream, data[x].bitmapDataStream, data[x].bitmapLength);
		}
	}

	return dst;
fail:
	free_bitmap_data(dst, count);
	return NULL;
}