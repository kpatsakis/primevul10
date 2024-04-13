ins_compl_key2count(int c)
{
    int		h;

    if (ins_compl_pum_key(c) && c != K_UP && c != K_DOWN)
    {
	h = pum_get_height();
	if (h > 3)
	    h -= 2; // keep some context
	return h;
    }
    return 1;
}