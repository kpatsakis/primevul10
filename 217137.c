pum_enough_matches(void)
{
    compl_T     *compl;
    int		i;

    // Don't display the popup menu if there are no matches or there is only
    // one (ignoring the original text).
    compl = compl_first_match;
    i = 0;
    do
    {
	if (compl == NULL || (!match_at_original_text(compl) && ++i == 2))
	    break;
	compl = compl->cp_next;
    } while (!is_first_match(compl));

    if (strstr((char *)p_cot, "menuone") != NULL)
	return (i >= 1);
    return (i >= 2);
}