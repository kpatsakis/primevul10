POINTER_CACHED_UPDATE* update_read_pointer_cached(rdpUpdate* update, wStream* s)
{
	POINTER_CACHED_UPDATE* pointer = calloc(1, sizeof(POINTER_CACHED_UPDATE));

	if (!pointer)
		goto fail;

	if (Stream_GetRemainingLength(s) < 2)
		goto fail;

	Stream_Read_UINT16(s, pointer->cacheIndex); /* cacheIndex (2 bytes) */
	return pointer;
fail:
	free_pointer_cached_update(update->context, pointer);
	return NULL;
}