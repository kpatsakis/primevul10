spell_back_to_badword(void)
{
    pos_T	tpos = curwin->w_cursor;

    spell_bad_len = spell_move_to(curwin, BACKWARD, TRUE, TRUE, NULL);
    if (curwin->w_cursor.col != tpos.col)
	start_arrow(&tpos);
}