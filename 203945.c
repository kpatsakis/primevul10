extract_resource (WinLibrary *fi, WinResource *wr, int *size,
                  bool *free_it, char *type, char *lang, bool raw)
{
	char *str;
	int32_t intval;
	
	/* just return pointer to data if raw */
	if (raw) {
		*free_it = false;
		/* get_resource_entry will print possible error */
		return get_resource_entry(fi, wr, size);
	}

	/* find out how to extract */
	str = type;
	if (str != NULL && parse_int32(STRIP_RES_ID_FORMAT(str), &intval)) {
		if (intval == (int) RT_BITMAP) {
			*free_it = true;
			return extract_bitmap_resource(fi, wr, size);
		}
		if (intval == (int) RT_GROUP_ICON) {
			*free_it = true;
			return extract_group_icon_cursor_resource(fi, wr, lang, size, true);
		}
		if (intval == (int) RT_GROUP_CURSOR) {
			*free_it = true;
			return extract_group_icon_cursor_resource(fi, wr, lang, size, false);
		}
	}

	warn(_("%s: don't know how to extract resource, try `--raw'"), fi->name);
	return NULL;
}