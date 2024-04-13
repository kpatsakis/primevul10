void bitmap_cache_register_callbacks(rdpUpdate* update)
{
	rdpCache* cache = update->context->cache;
	cache->bitmap->MemBlt = update->primary->MemBlt;
	cache->bitmap->Mem3Blt = update->primary->Mem3Blt;
	update->primary->MemBlt = update_gdi_memblt;
	update->primary->Mem3Blt = update_gdi_mem3blt;
	update->secondary->CacheBitmap = update_gdi_cache_bitmap;
	update->secondary->CacheBitmapV2 = update_gdi_cache_bitmap_v2;
	update->secondary->CacheBitmapV3 = update_gdi_cache_bitmap_v3;
	update->BitmapUpdate = gdi_bitmap_update;
}