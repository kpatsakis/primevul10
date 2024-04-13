get_complete_info(list_T *what_list, dict_T *retdict)
{
    int		ret = OK;
    listitem_T	*item;
#define CI_WHAT_MODE		0x01
#define CI_WHAT_PUM_VISIBLE	0x02
#define CI_WHAT_ITEMS		0x04
#define CI_WHAT_SELECTED	0x08
#define CI_WHAT_INSERTED	0x10
#define CI_WHAT_ALL		0xff
    int		what_flag;

    if (what_list == NULL)
	what_flag = CI_WHAT_ALL;
    else
    {
	what_flag = 0;
	CHECK_LIST_MATERIALIZE(what_list);
	FOR_ALL_LIST_ITEMS(what_list, item)
	{
	    char_u *what = tv_get_string(&item->li_tv);

	    if (STRCMP(what, "mode") == 0)
		what_flag |= CI_WHAT_MODE;
	    else if (STRCMP(what, "pum_visible") == 0)
		what_flag |= CI_WHAT_PUM_VISIBLE;
	    else if (STRCMP(what, "items") == 0)
		what_flag |= CI_WHAT_ITEMS;
	    else if (STRCMP(what, "selected") == 0)
		what_flag |= CI_WHAT_SELECTED;
	    else if (STRCMP(what, "inserted") == 0)
		what_flag |= CI_WHAT_INSERTED;
	}
    }

    if (ret == OK && (what_flag & CI_WHAT_MODE))
	ret = dict_add_string(retdict, "mode", ins_compl_mode());

    if (ret == OK && (what_flag & CI_WHAT_PUM_VISIBLE))
	ret = dict_add_number(retdict, "pum_visible", pum_visible());

    if (ret == OK && (what_flag & CI_WHAT_ITEMS))
    {
	list_T	    *li;
	dict_T	    *di;
	compl_T     *match;

	li = list_alloc();
	if (li == NULL)
	    return;
	ret = dict_add_list(retdict, "items", li);
	if (ret == OK && compl_first_match != NULL)
	{
	    match = compl_first_match;
	    do
	    {
		if (!match_at_original_text(match))
		{
		    di = dict_alloc();
		    if (di == NULL)
			return;
		    ret = list_append_dict(li, di);
		    if (ret != OK)
			return;
		    dict_add_string(di, "word", match->cp_str);
		    dict_add_string(di, "abbr", match->cp_text[CPT_ABBR]);
		    dict_add_string(di, "menu", match->cp_text[CPT_MENU]);
		    dict_add_string(di, "kind", match->cp_text[CPT_KIND]);
		    dict_add_string(di, "info", match->cp_text[CPT_INFO]);
		    if (match->cp_user_data.v_type == VAR_UNKNOWN)
			// Add an empty string for backwards compatibility
			dict_add_string(di, "user_data", (char_u *)"");
		    else
			dict_add_tv(di, "user_data", &match->cp_user_data);
		}
		match = match->cp_next;
	    }
	    while (match != NULL && !is_first_match(match));
	}
    }

    if (ret == OK && (what_flag & CI_WHAT_SELECTED))
    {
	if (compl_curr_match != NULL && compl_curr_match->cp_number == -1)
	    ins_compl_update_sequence_numbers();
	ret = dict_add_number(retdict, "selected", compl_curr_match != NULL
				      ? compl_curr_match->cp_number - 1 : -1);
    }

    // TODO
    // if (ret == OK && (what_flag & CI_WHAT_INSERTED))
}