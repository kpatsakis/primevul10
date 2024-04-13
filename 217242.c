get_next_include_file_completion(int compl_type)
{
    find_pattern_in_path(compl_pattern, compl_direction,
	    (int)STRLEN(compl_pattern), FALSE, FALSE,
	    (compl_type == CTRL_X_PATH_DEFINES
	     && !(compl_cont_status & CONT_SOL))
	    ? FIND_DEFINE : FIND_ANY, 1L, ACTION_EXPAND,
	    (linenr_T)1, (linenr_T)MAXLNUM);
}