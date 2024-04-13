ins_compl_dictionaries(
    char_u	*dict_start,
    char_u	*pat,
    int		flags,		// DICT_FIRST and/or DICT_EXACT
    int		thesaurus)	// Thesaurus completion
{
    char_u	*dict = dict_start;
    char_u	*ptr;
    char_u	*buf;
    regmatch_T	regmatch;
    char_u	**files;
    int		count;
    int		save_p_scs;
    int		dir = compl_direction;

    if (*dict == NUL)
    {
#ifdef FEAT_SPELL
	// When 'dictionary' is empty and spell checking is enabled use
	// "spell".
	if (!thesaurus && curwin->w_p_spell)
	    dict = (char_u *)"spell";
	else
#endif
	    return;
    }

    buf = alloc(LSIZE);
    if (buf == NULL)
	return;
    regmatch.regprog = NULL;	// so that we can goto theend

    // If 'infercase' is set, don't use 'smartcase' here
    save_p_scs = p_scs;
    if (curbuf->b_p_inf)
	p_scs = FALSE;

    // When invoked to match whole lines for CTRL-X CTRL-L adjust the pattern
    // to only match at the start of a line.  Otherwise just match the
    // pattern. Also need to double backslashes.
    if (ctrl_x_mode_line_or_eval())
    {
	char_u *pat_esc = vim_strsave_escaped(pat, (char_u *)"\\");
	size_t len;

	if (pat_esc == NULL)
	    goto theend;
	len = STRLEN(pat_esc) + 10;
	ptr = alloc(len);
	if (ptr == NULL)
	{
	    vim_free(pat_esc);
	    goto theend;
	}
	vim_snprintf((char *)ptr, len, "^\\s*\\zs\\V%s", pat_esc);
	regmatch.regprog = vim_regcomp(ptr, RE_MAGIC);
	vim_free(pat_esc);
	vim_free(ptr);
    }
    else
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	if (regmatch.regprog == NULL)
	    goto theend;
    }

    // ignore case depends on 'ignorecase', 'smartcase' and "pat"
    regmatch.rm_ic = ignorecase(pat);
    while (*dict != NUL && !got_int && !compl_interrupted)
    {
	// copy one dictionary file name into buf
	if (flags == DICT_EXACT)
	{
	    count = 1;
	    files = &dict;
	}
	else
	{
	    // Expand wildcards in the dictionary name, but do not allow
	    // backticks (for security, the 'dict' option may have been set in
	    // a modeline).
	    copy_option_part(&dict, buf, LSIZE, ",");
# ifdef FEAT_SPELL
	    if (!thesaurus && STRCMP(buf, "spell") == 0)
		count = -1;
	    else
# endif
		if (vim_strchr(buf, '`') != NULL
		    || expand_wildcards(1, &buf, &count, &files,
						     EW_FILE|EW_SILENT) != OK)
		count = 0;
	}

# ifdef FEAT_SPELL
	if (count == -1)
	{
	    // Complete from active spelling.  Skip "\<" in the pattern, we
	    // don't use it as a RE.
	    if (pat[0] == '\\' && pat[1] == '<')
		ptr = pat + 2;
	    else
		ptr = pat;
	    spell_dump_compl(ptr, regmatch.rm_ic, &dir, 0);
	}
	else
# endif
	    if (count > 0)	// avoid warning for using "files" uninit
	{
	    ins_compl_files(count, files, thesaurus, flags,
							&regmatch, buf, &dir);
	    if (flags != DICT_EXACT)
		FreeWild(count, files);
	}
	if (flags != 0)
	    break;
    }

theend:
    p_scs = save_p_scs;
    vim_regfree(regmatch.regprog);
    vim_free(buf);
}