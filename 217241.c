ins_compl_del_pum(void)
{
    if (compl_match_array == NULL)
	return;

    pum_undisplay();
    VIM_CLEAR(compl_match_array);
}