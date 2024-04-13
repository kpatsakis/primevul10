u_undoline(void)
{
    colnr_T t;
    char_u  *oldp;

    if (undo_off)
	return;

    if (curbuf->b_u_line_ptr == NULL
			|| curbuf->b_u_line_lnum > curbuf->b_ml.ml_line_count)
    {
	beep_flush();
	return;
    }

    /* first save the line for the 'u' command */
    if (u_savecommon(curbuf->b_u_line_lnum - 1,
		       curbuf->b_u_line_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	return;
    oldp = u_save_line(curbuf->b_u_line_lnum);
    if (oldp == NULL)
    {
	do_outofmem_msg((long_u)0);
	return;
    }
    ml_replace(curbuf->b_u_line_lnum, curbuf->b_u_line_ptr, TRUE);
    changed_bytes(curbuf->b_u_line_lnum, 0);
    vim_free(curbuf->b_u_line_ptr);
    curbuf->b_u_line_ptr = oldp;

    t = curbuf->b_u_line_colnr;
    if (curwin->w_cursor.lnum == curbuf->b_u_line_lnum)
	curbuf->b_u_line_colnr = curwin->w_cursor.col;
    curwin->w_cursor.col = t;
    curwin->w_cursor.lnum = curbuf->b_u_line_lnum;
    check_cursor_col();
}