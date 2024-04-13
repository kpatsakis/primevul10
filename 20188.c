alist_expand(int *fnum_list, int fnum_len)
{
    char_u	**old_arg_files;
    int		old_arg_count;
    char_u	**new_arg_files;
    int		new_arg_file_count;
    char_u	*save_p_su = p_su;
    int		i;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = empty_option;
    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    if (old_arg_files != NULL)
    {
	for (i = 0; i < GARGCOUNT; ++i)
	    old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
	old_arg_count = GARGCOUNT;
	if (expand_wildcards(old_arg_count, old_arg_files,
		    &new_arg_file_count, &new_arg_files,
		    EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
		&& new_arg_file_count > 0)
	{
	    alist_set(&global_alist, new_arg_file_count, new_arg_files,
						   TRUE, fnum_list, fnum_len);
	    FreeWild(old_arg_count, old_arg_files);
	}
    }
    p_su = save_p_su;
}