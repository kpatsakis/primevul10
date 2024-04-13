get_undolevel(void)
{
    if (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	return p_ul;
    return curbuf->b_p_ul;
}