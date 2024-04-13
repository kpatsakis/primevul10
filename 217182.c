ins_compl_build_pum(void)
{
    compl_T     *compl;
    compl_T     *shown_compl = NULL;
    int		did_find_shown_match = FALSE;
    int		shown_match_ok = FALSE;
    int		i;
    int		cur = -1;
    int		lead_len = 0;

    // Need to build the popup menu list.
    compl_match_arraysize = 0;
    compl = compl_first_match;
    if (compl_leader != NULL)
	lead_len = (int)STRLEN(compl_leader);

    do
    {
	if (!match_at_original_text(compl)
		&& (compl_leader == NULL
		    || ins_compl_equal(compl, compl_leader, lead_len)))
	    ++compl_match_arraysize;
	compl = compl->cp_next;
    } while (compl != NULL && !is_first_match(compl));

    if (compl_match_arraysize == 0)
	return -1;

    compl_match_array = ALLOC_CLEAR_MULT(pumitem_T, compl_match_arraysize);
    if (compl_match_array == NULL)
	return -1;

    // If the current match is the original text don't find the first
    // match after it, don't highlight anything.
    if (match_at_original_text(compl_shown_match))
	shown_match_ok = TRUE;

    i = 0;
    compl = compl_first_match;
    do
    {
	if (!match_at_original_text(compl)
		&& (compl_leader == NULL
		    || ins_compl_equal(compl, compl_leader, lead_len)))
	{
	    if (!shown_match_ok)
	    {
		if (compl == compl_shown_match || did_find_shown_match)
		{
		    // This item is the shown match or this is the
		    // first displayed item after the shown match.
		    compl_shown_match = compl;
		    did_find_shown_match = TRUE;
		    shown_match_ok = TRUE;
		}
		else
		    // Remember this displayed match for when the
		    // shown match is just below it.
		    shown_compl = compl;
		cur = i;
	    }

	    if (compl->cp_text[CPT_ABBR] != NULL)
		compl_match_array[i].pum_text =
		    compl->cp_text[CPT_ABBR];
	    else
		compl_match_array[i].pum_text = compl->cp_str;
	    compl_match_array[i].pum_kind = compl->cp_text[CPT_KIND];
	    compl_match_array[i].pum_info = compl->cp_text[CPT_INFO];
	    if (compl->cp_text[CPT_MENU] != NULL)
		compl_match_array[i++].pum_extra =
		    compl->cp_text[CPT_MENU];
	    else
		compl_match_array[i++].pum_extra = compl->cp_fname;
	}

	if (compl == compl_shown_match)
	{
	    did_find_shown_match = TRUE;

	    // When the original text is the shown match don't set
	    // compl_shown_match.
	    if (match_at_original_text(compl))
		shown_match_ok = TRUE;

	    if (!shown_match_ok && shown_compl != NULL)
	    {
		// The shown match isn't displayed, set it to the
		// previously displayed match.
		compl_shown_match = shown_compl;
		shown_match_ok = TRUE;
	    }
	}
	compl = compl->cp_next;
    } while (compl != NULL && !is_first_match(compl));

    if (!shown_match_ok)    // no displayed match at all
	cur = -1;

    return cur;
}