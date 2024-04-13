ins_compl_dict_alloc(compl_T *match)
{
    dict_T *dict = dict_alloc_lock(VAR_FIXED);

    if (dict == NULL)
	return NULL;

    dict_add_string(dict, "word", match->cp_str);
    dict_add_string(dict, "abbr", match->cp_text[CPT_ABBR]);
    dict_add_string(dict, "menu", match->cp_text[CPT_MENU]);
    dict_add_string(dict, "kind", match->cp_text[CPT_KIND]);
    dict_add_string(dict, "info", match->cp_text[CPT_INFO]);
    if (match->cp_user_data.v_type == VAR_UNKNOWN)
	dict_add_string(dict, "user_data", (char_u *)"");
    else
	dict_add_tv(dict, "user_data", &match->cp_user_data);

    return dict;
}