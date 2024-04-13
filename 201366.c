sel_pos(int n)
{
	return inverse_translate(sel_cons, screen_glyph(sel_cons, n),
				use_unicode);
}