get_next_dict_tsr_completion(int compl_type, char_u *dict, int dict_f)
{
#ifdef FEAT_COMPL_FUNC
    if (thesaurus_func_complete(compl_type))
	expand_by_function(compl_type, compl_pattern);
    else
#endif
	ins_compl_dictionaries(
		dict != NULL ? dict
		: (compl_type == CTRL_X_THESAURUS
		    ? (*curbuf->b_p_tsr == NUL ? p_tsr : curbuf->b_p_tsr)
		    : (*curbuf->b_p_dict == NUL ? p_dict : curbuf->b_p_dict)),
		compl_pattern,
		dict != NULL ? dict_f : 0,
		compl_type == CTRL_X_THESAURUS);
}