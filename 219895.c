u_get_undo_file_name(char_u *buf_ffname, int reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    char_u	*munged_name = NULL;
    char_u	*undo_file_name = NULL;
    int		dir_len;
    char_u	*p;
    stat_T	st;
    char_u	*ffname = buf_ffname;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#endif

    if (ffname == NULL)
	return NULL;

#ifdef HAVE_READLINK
    /* Expand symlink in the file name, so that we put the undo file with the
     * actual file instead of with the symlink. */
    if (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
#endif

    /* Loop over 'undodir'.  When reading find the first file that exists.
     * When not reading use the first directory that exists or ".". */
    dirp = p_udir;
    while (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	if (dir_len == 1 && dir_name[0] == '.')
	{
	    /* Use same directory as the ffname,
	     * "dir/name" -> "dir/.name.un~" */
	    undo_file_name = vim_strnsave(ffname, (int)(STRLEN(ffname) + 5));
	    if (undo_file_name == NULL)
		break;
	    p = gettail(undo_file_name);
#ifdef VMS
	    /* VMS can not handle more than one dot in the filenames
	     * use "dir/name" -> "dir/_un_name" - add _un_
	     * at the beginning to keep the extension */
	    mch_memmove(p + 4,  p, STRLEN(p) + 1);
	    mch_memmove(p, "_un_", 4);

#else
	    /* Use same directory as the ffname,
	     * "dir/name" -> "dir/.name.un~" */
	    mch_memmove(p + 1, p, STRLEN(p) + 1);
	    *p = '.';
	    STRCAT(p, ".un~");
#endif
	}
	else
	{
	    dir_name[dir_len] = NUL;
	    if (mch_isdir(dir_name))
	    {
		if (munged_name == NULL)
		{
		    munged_name = vim_strsave(ffname);
		    if (munged_name == NULL)
			return NULL;
		    for (p = munged_name; *p != NUL; mb_ptr_adv(p))
			if (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, munged_name, TRUE);
	    }
	}

	/* When reading check if the file exists. */
	if (undo_file_name != NULL && (!reading
			       || mch_stat((char *)undo_file_name, &st) >= 0))
	    break;
	vim_free(undo_file_name);
	undo_file_name = NULL;
    }

    vim_free(munged_name);
    return undo_file_name;
}