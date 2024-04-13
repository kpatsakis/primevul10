do_arglist(
    char_u	*str,
    int		what,
    int		after UNUSED,	// 0 means before first one
    int		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    int		exp_count;
    char_u	**exp_files;
    int		i;
    char_u	*p;
    int		match;
    int		arg_escaped = TRUE;

    if (check_arglist_locked() == FAIL)
	return FAIL;

    // Set default argument for ":argadd" command.
    if (what == AL_ADD && *str == NUL)
    {
	if (curbuf->b_ffname == NULL)
	    return FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    if (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	return FAIL;

    if (what == AL_DEL)
    {
	regmatch_T	regmatch;
	int		didone;

	// Delete the items: use each item as a regexp and find a match in the
	// argument list.
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
	for (i = 0; i < new_ga.ga_len && !got_int; ++i)
	{
	    p = ((char_u **)new_ga.ga_data)[i];
	    p = file_pat_to_reg_pat(p, NULL, NULL, FALSE);
	    if (p == NULL)
		break;
	    regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	    if (regmatch.regprog == NULL)
	    {
		vim_free(p);
		break;
	    }

	    didone = FALSE;
	    for (match = 0; match < ARGCOUNT; ++match)
		if (vim_regexec(&regmatch, alist_name(&ARGLIST[match]),
								  (colnr_T)0))
		{
		    didone = TRUE;
		    vim_free(ARGLIST[match].ae_fname);
		    mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			    (ARGCOUNT - match - 1) * sizeof(aentry_T));
		    --ALIST(curwin)->al_ga.ga_len;
		    if (curwin->w_arg_idx > match)
			--curwin->w_arg_idx;
		    --match;
		}

	    vim_regfree(regmatch.regprog);
	    vim_free(p);
	    if (!didone)
		semsg(_(e_nomatch2), ((char_u **)new_ga.ga_data)[i]);
	}
	ga_clear(&new_ga);
    }
    else
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_count, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	if (i == FAIL || exp_count == 0)
	{
	    emsg(_(e_nomatch));
	    return FAIL;
	}

	if (what == AL_ADD)
	{
	    alist_add_list(exp_count, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	else // what == AL_SET
	    alist_set(ALIST(curwin), exp_count, exp_files, will_edit, NULL, 0);
    }

    alist_check_arg_idx();

    return OK;
}