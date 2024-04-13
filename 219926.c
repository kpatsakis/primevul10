u_update_save_nr(buf_T *buf)
{
    u_header_T	*uhp;

    ++buf->b_u_save_nr_last;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_last;
    uhp = buf->b_u_curhead;
    if (uhp != NULL)
	uhp = uhp->uh_next.ptr;
    else
	uhp = buf->b_u_newhead;
    if (uhp != NULL)
	uhp->uh_save_nr = buf->b_u_save_nr_last;
}