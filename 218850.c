void update_reset_state(rdpUpdate* update)
{
	rdpPrimaryUpdate* primary = update->primary;
	rdpAltSecUpdate* altsec = update->altsec;

	if (primary->fast_glyph.glyphData.aj)
	{
		free(primary->fast_glyph.glyphData.aj);
		primary->fast_glyph.glyphData.aj = NULL;
	}

	ZeroMemory(&primary->order_info, sizeof(ORDER_INFO));
	ZeroMemory(&primary->dstblt, sizeof(DSTBLT_ORDER));
	ZeroMemory(&primary->patblt, sizeof(PATBLT_ORDER));
	ZeroMemory(&primary->scrblt, sizeof(SCRBLT_ORDER));
	ZeroMemory(&primary->opaque_rect, sizeof(OPAQUE_RECT_ORDER));
	ZeroMemory(&primary->draw_nine_grid, sizeof(DRAW_NINE_GRID_ORDER));
	ZeroMemory(&primary->multi_dstblt, sizeof(MULTI_DSTBLT_ORDER));
	ZeroMemory(&primary->multi_patblt, sizeof(MULTI_PATBLT_ORDER));
	ZeroMemory(&primary->multi_scrblt, sizeof(MULTI_SCRBLT_ORDER));
	ZeroMemory(&primary->multi_opaque_rect, sizeof(MULTI_OPAQUE_RECT_ORDER));
	ZeroMemory(&primary->multi_draw_nine_grid, sizeof(MULTI_DRAW_NINE_GRID_ORDER));
	ZeroMemory(&primary->line_to, sizeof(LINE_TO_ORDER));
	ZeroMemory(&primary->polyline, sizeof(POLYLINE_ORDER));
	ZeroMemory(&primary->memblt, sizeof(MEMBLT_ORDER));
	ZeroMemory(&primary->mem3blt, sizeof(MEM3BLT_ORDER));
	ZeroMemory(&primary->save_bitmap, sizeof(SAVE_BITMAP_ORDER));
	ZeroMemory(&primary->glyph_index, sizeof(GLYPH_INDEX_ORDER));
	ZeroMemory(&primary->fast_index, sizeof(FAST_INDEX_ORDER));
	ZeroMemory(&primary->fast_glyph, sizeof(FAST_GLYPH_ORDER));
	ZeroMemory(&primary->polygon_sc, sizeof(POLYGON_SC_ORDER));
	ZeroMemory(&primary->polygon_cb, sizeof(POLYGON_CB_ORDER));
	ZeroMemory(&primary->ellipse_sc, sizeof(ELLIPSE_SC_ORDER));
	ZeroMemory(&primary->ellipse_cb, sizeof(ELLIPSE_CB_ORDER));
	primary->order_info.orderType = ORDER_TYPE_PATBLT;

	if (!update->initialState)
	{
		altsec->switch_surface.bitmapId = SCREEN_BITMAP_SURFACE;
		IFCALL(altsec->SwitchSurface, update->context, &(altsec->switch_surface));
	}
}