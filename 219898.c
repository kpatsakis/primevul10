serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_bytes(bi, (long_u)pos.lnum, 4);
    undo_write_bytes(bi, (long_u)pos.col, 4);
#ifdef FEAT_VIRTUALEDIT
    undo_write_bytes(bi, (long_u)pos.coladd, 4);
#else
    undo_write_bytes(bi, (long_u)0, 4);
#endif
}