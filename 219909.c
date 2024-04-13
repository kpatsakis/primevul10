u_clearline(void)
{
    if (curbuf->b_u_line_ptr != NULL)
    {
	vim_free(curbuf->b_u_line_ptr);
	curbuf->b_u_line_ptr = NULL;
	curbuf->b_u_line_lnum = 0;
    }
}