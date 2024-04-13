BITMAP_UPDATE* copy_bitmap_update(rdpContext* context, const BITMAP_UPDATE* pointer)
{
	BITMAP_UPDATE* dst = calloc(1, sizeof(BITMAP_UPDATE));

	if (!dst || !pointer)
		goto fail;

	*dst = *pointer;
	dst->rectangles = copy_bitmap_data(pointer->rectangles, pointer->number);

	if (!dst->rectangles)
		goto fail;

	return dst;
fail:
	free_bitmap_update(context, dst);
	return NULL;
}