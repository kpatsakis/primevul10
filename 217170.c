get_next_default_completion(ins_compl_next_state_T *st, pos_T *start_pos)
{
    int		found_new_match = FAIL;
    int		save_p_scs;
    int		save_p_ws;
    int		looped_around = FALSE;
    char_u	*ptr;
    int		len;

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    if (st->ins_buf->b_p_inf)
	p_scs = FALSE;

    //	Buffers other than curbuf are scanned from the beginning or the
    //	end but never from the middle, thus setting nowrapscan in this
    //	buffer is a good idea, on the other hand, we always set
    //	wrapscan for curbuf to avoid missing matches -- Acevedo,Webb
    save_p_ws = p_ws;
    if (st->ins_buf != curbuf)
	p_ws = FALSE;
    else if (*st->e_cpt == '.')
	p_ws = TRUE;
    looped_around = FALSE;
    for (;;)
    {
	int	cont_s_ipos = FALSE;

	++msg_silent;  // Don't want messages for wrapscan.

	// ctrl_x_mode_line_or_eval() || word-wise search that
	// has added a word that was at the beginning of the line
	if (ctrl_x_mode_line_or_eval() || (compl_cont_status & CONT_SOL))
	    found_new_match = search_for_exact_line(st->ins_buf,
			    st->cur_match_pos, compl_direction, compl_pattern);
	else
	    found_new_match = searchit(NULL, st->ins_buf, st->cur_match_pos,
				NULL, compl_direction, compl_pattern, 1L,
				SEARCH_KEEP + SEARCH_NFMSG, RE_LAST, NULL);
	--msg_silent;
	if (!compl_started || st->set_match_pos)
	{
	    // set "compl_started" even on fail
	    compl_started = TRUE;
	    st->first_match_pos = *st->cur_match_pos;
	    st->last_match_pos = *st->cur_match_pos;
	    st->set_match_pos = FALSE;
	}
	else if (st->first_match_pos.lnum == st->last_match_pos.lnum
		&& st->first_match_pos.col == st->last_match_pos.col)
	{
	    found_new_match = FAIL;
	}
	else if (compl_dir_forward()
		&& (st->prev_match_pos.lnum > st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col >= st->cur_match_pos->col)))
	{
	    if (looped_around)
		found_new_match = FAIL;
	    else
		looped_around = TRUE;
	}
	else if (!compl_dir_forward()
		&& (st->prev_match_pos.lnum < st->cur_match_pos->lnum
		    || (st->prev_match_pos.lnum == st->cur_match_pos->lnum
			&& st->prev_match_pos.col <= st->cur_match_pos->col)))
	{
	    if (looped_around)
		found_new_match = FAIL;
	    else
		looped_around = TRUE;
	}
	st->prev_match_pos = *st->cur_match_pos;
	if (found_new_match == FAIL)
	    break;

	// when ADDING, the text before the cursor matches, skip it
	if (compl_status_adding() && st->ins_buf == curbuf
		&& start_pos->lnum == st->cur_match_pos->lnum
		&& start_pos->col  == st->cur_match_pos->col)
	    continue;

	ptr = ins_comp_get_next_word_or_line(st->ins_buf, st->cur_match_pos,
							&len, &cont_s_ipos);
	if (ptr == NULL)
	    continue;

	if (ins_compl_add_infercase(ptr, len, p_ic,
		    st->ins_buf == curbuf ? NULL : st->ins_buf->b_sfname,
		    0, cont_s_ipos) != NOTDONE)
	{
	    found_new_match = OK;
	    break;
	}
    }
    p_scs = save_p_scs;
    p_ws = save_p_ws;

    return found_new_match;
}