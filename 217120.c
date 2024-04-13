pum_wanted(void)
{
    // 'completeopt' must contain "menu" or "menuone"
    if (vim_strchr(p_cot, 'm') == NULL)
	return FALSE;

    // The display looks bad on a B&W display.
    if (t_colors < 8
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
	return FALSE;
    return TRUE;
}