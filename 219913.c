unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    if (pos->lnum < 0)
	pos->lnum = 0;
    pos->col = undo_read_4c(bi);
    if (pos->col < 0)
	pos->col = 0;
#ifdef FEAT_VIRTUALEDIT
    pos->coladd = undo_read_4c(bi);
    if (pos->coladd < 0)
	pos->coladd = 0;
#else
    (void)undo_read_4c(bi);
#endif
}