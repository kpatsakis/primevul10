ins_compl_show_pum(void)
{
    int		i;
    int		cur = -1;
    colnr_T	col;

    if (!pum_wanted() || !pum_enough_matches())
	return;

#if defined(FEAT_EVAL)
    // Dirty hard-coded hack: remove any matchparen highlighting.
    do_cmdline_cmd((char_u *)"if exists('g:loaded_matchparen')|:3match none|endif");
#endif

    // Update the screen later, before drawing the popup menu over it.
    pum_call_update_screen();

    if (compl_match_array == NULL)
	// Need to build the popup menu list.
	cur = ins_compl_build_pum();
    else
    {
	// popup menu already exists, only need to find the current item.
	for (i = 0; i < compl_match_arraysize; ++i)
	    if (compl_match_array[i].pum_text == compl_shown_match->cp_str
		    || compl_match_array[i].pum_text
				      == compl_shown_match->cp_text[CPT_ABBR])
	    {
		cur = i;
		break;
	    }
    }

    if (compl_match_array == NULL)
	return;

    // In Replace mode when a $ is displayed at the end of the line only
    // part of the screen would be updated.  We do need to redraw here.
    dollar_vcol = -1;

    // Compute the screen column of the start of the completed text.
    // Use the cursor to get all wrapping and other settings right.
    col = curwin->w_cursor.col;
    curwin->w_cursor.col = compl_col;
    pum_display(compl_match_array, compl_match_arraysize, cur);
    curwin->w_cursor.col = col;

#ifdef FEAT_EVAL
    if (has_completechanged())
	trigger_complete_changed_event(cur);
#endif
}