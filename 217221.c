ins_compl_long_shown_match(void)
{
    return (int)STRLEN(compl_shown_match->cp_str)
					    > curwin->w_cursor.col - compl_col;
}