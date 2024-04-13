ins_compl_fixRedoBufForLeader(char_u *ptr_arg)
{
    int	    len;
    char_u  *p;
    char_u  *ptr = ptr_arg;

    if (ptr == NULL)
    {
	if (compl_leader != NULL)
	    ptr = compl_leader;
	else
	    return;  // nothing to do
    }
    if (compl_orig_text != NULL)
    {
	p = compl_orig_text;
	for (len = 0; p[len] != NUL && p[len] == ptr[len]; ++len)
	    ;
	if (len > 0)
	    len -= (*mb_head_off)(p, p + len);
	for (p += len; *p != NUL; MB_PTR_ADV(p))
	    AppendCharToRedobuff(K_BS);
    }
    else
	len = 0;
    if (ptr != NULL)
	AppendToRedobuffLit(ptr + len, -1);
}