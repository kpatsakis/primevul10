bufIsChanged(buf_T *buf)
{
    return
#ifdef FEAT_QUICKFIX
	    !bt_dontwrite(buf) &&
#endif
	    (buf->b_changed || file_ff_differs(buf, TRUE));
}