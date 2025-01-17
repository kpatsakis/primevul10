u_saveline(linenr_T lnum)
{
    if (lnum == curbuf->b_u_line_lnum)	    /* line is already saved */
	return;
    if (lnum < 1 || lnum > curbuf->b_ml.ml_line_count) /* should never happen */
	return;
    u_clearline();
    curbuf->b_u_line_lnum = lnum;
    if (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_line_colnr = curwin->w_cursor.col;
    else
	curbuf->b_u_line_colnr = 0;
    if ((curbuf->b_u_line_ptr = u_save_line(lnum)) == NULL)
	do_outofmem_msg((long_u)0);
}