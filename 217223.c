ins_compl_delete(void)
{
    int	    col;

    // In insert mode: Delete the typed part.
    // In replace mode: Put the old characters back, if any.
    col = compl_col + (compl_status_adding() ? compl_length : 0);
    if ((int)curwin->w_cursor.col > col)
    {
	if (stop_arrow() == FAIL)
	    return;
	backspace_until_column(col);
    }

    // TODO: is this sufficient for redrawing?  Redrawing everything causes
    // flicker, thus we can't do that.
    changed_cline_bef_curs();
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}