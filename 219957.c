curbufIsChanged(void)
{
    return
#ifdef FEAT_QUICKFIX
	!bt_dontwrite(curbuf) &&
#endif
	(curbuf->b_changed || file_ff_differs(curbuf, TRUE));
}