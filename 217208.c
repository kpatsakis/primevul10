ins_compl_new_leader(void)
{
    ins_compl_del_pum();
    ins_compl_delete();
    ins_bytes(compl_leader + get_compl_len());
    compl_used_match = FALSE;

    if (compl_started)
	ins_compl_set_original_text(compl_leader);
    else
    {
#ifdef FEAT_SPELL
	spell_bad_len = 0;	// need to redetect bad word
#endif
	// Matches were cleared, need to search for them now.  Before drawing
	// the popup menu display the changed text before the cursor.  Set
	// "compl_restarting" to avoid that the first match is inserted.
	pum_call_update_screen();
#ifdef FEAT_GUI
	if (gui.in_use)
	{
	    // Show the cursor after the match, not after the redrawn text.
	    setcursor();
	    out_flush_cursor(FALSE, FALSE);
	}
#endif
	compl_restarting = TRUE;
	if (ins_complete(Ctrl_N, TRUE) == FAIL)
	    compl_cont_status = 0;
	compl_restarting = FALSE;
    }

    compl_enter_selects = !compl_used_match;

    // Show the popup menu with a different set of matches.
    ins_compl_show_pum();

    // Don't let Enter select the original text when there is no popup menu.
    if (compl_match_array == NULL)
	compl_enter_selects = FALSE;
}