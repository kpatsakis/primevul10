completeopt_was_set(void)
{
    compl_no_insert = FALSE;
    compl_no_select = FALSE;
    if (strstr((char *)p_cot, "noselect") != NULL)
	compl_no_select = TRUE;
    if (strstr((char *)p_cot, "noinsert") != NULL)
	compl_no_insert = TRUE;
}