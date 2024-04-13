set_hunkmax (void)
{
    if (!p_line)
	p_line = xmalloc (hunkmax * sizeof *p_line);
    if (!p_len)
	p_len = xmalloc (hunkmax * sizeof *p_len);
    if (!p_Char)
	p_Char = xmalloc (hunkmax * sizeof *p_Char);
}