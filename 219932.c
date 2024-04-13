u_savedel(linenr_T lnum, long nlines)
{
    if (undo_off)
	return OK;

    return (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_line_count ? 2 : lnum, FALSE));
}