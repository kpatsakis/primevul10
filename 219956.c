u_inssub(linenr_T lnum)
{
    if (undo_off)
	return OK;

    return (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}