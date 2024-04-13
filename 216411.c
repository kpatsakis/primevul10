void free_bitmap_update(rdpContext* context, BITMAP_UPDATE* pointer)
{
	if (!pointer)
		return;

	free_bitmap_data(pointer->rectangles, pointer->number);
	free(pointer);
}