ins_compl_key2dir(int c)
{
    if (c == Ctrl_P || c == Ctrl_L
	    || c == K_PAGEUP || c == K_KPAGEUP || c == K_S_UP || c == K_UP)
	return BACKWARD;
    return FORWARD;
}