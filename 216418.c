BOOL bitmap_cache_put(rdpBitmapCache* bitmapCache, UINT32 id, UINT32 index, rdpBitmap* bitmap)
{
	if (id > bitmapCache->maxCells)
	{
		WLog_ERR(TAG, "put invalid bitmap cell id: %" PRIu32 "", id);
		return FALSE;
	}

	if (index == BITMAP_CACHE_WAITING_LIST_INDEX)
	{
		index = bitmapCache->cells[id].number;
	}
	else if (index > bitmapCache->cells[id].number)
	{
		WLog_ERR(TAG, "put invalid bitmap index %" PRIu32 " in cell id: %" PRIu32 "", index, id);
		return FALSE;
	}

	bitmapCache->cells[id].entries[index] = bitmap;
	return TRUE;
}