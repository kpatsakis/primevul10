static const char *GetLanguage(char *lcode)
{
	s32 idx = gf_lang_find(lcode);
	if (idx>=0) return gf_lang_get_name(idx);
	return lcode;
}