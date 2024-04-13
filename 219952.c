u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = NULL;
    buf->b_u_synced = TRUE;
    buf->b_u_numhead = 0;
    buf->b_u_line_ptr = NULL;
    buf->b_u_line_lnum = 0;
}