u_eval_tree(u_header_T *first_uhp, list_T *list)
{
    u_header_T  *uhp = first_uhp;
    dict_T	*dict;

    while (uhp != NULL)
    {
	dict = dict_alloc();
	if (dict == NULL)
	    return;
	dict_add_nr_str(dict, "seq", uhp->uh_seq, NULL);
	dict_add_nr_str(dict, "time", (long)uhp->uh_time, NULL);
	if (uhp == curbuf->b_u_newhead)
	    dict_add_nr_str(dict, "newhead", 1, NULL);
	if (uhp == curbuf->b_u_curhead)
	    dict_add_nr_str(dict, "curhead", 1, NULL);
	if (uhp->uh_save_nr > 0)
	    dict_add_nr_str(dict, "save", uhp->uh_save_nr, NULL);

	if (uhp->uh_alt_next.ptr != NULL)
	{
	    list_T	*alt_list = list_alloc();

	    if (alt_list != NULL)
	    {
		/* Recursive call to add alternate undo tree. */
		u_eval_tree(uhp->uh_alt_next.ptr, alt_list);
		dict_add_list(dict, "alt", alt_list);
	    }
	}

	list_append_dict(list, dict);
	uhp = uhp->uh_prev.ptr;
    }
}