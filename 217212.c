ins_compl_has_shown_match(void)
{
    return compl_shown_match == NULL
	|| compl_shown_match != compl_shown_match->cp_next;
}