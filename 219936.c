u_write_undo(
    char_u	*name,
    int		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_header_T	*uhp;
    char_u	*file_name;
    int		mark;
#ifdef U_DEBUG
    int		headers_written = 0;
#endif
    int		fd;
    FILE	*fp = NULL;
    int		perm;
    int		write_ok = FALSE;
#ifdef UNIX
    int		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
#endif
    bufinfo_T	bi;

    vim_memset(&bi, 0, sizeof(bi));

    if (name == NULL)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	if (file_name == NULL)
	{
	    if (p_verbose > 0)
	    {
		verbose_enter();
		smsg((char_u *)
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    return;
	}
    }
    else
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    if (buf->b_ffname != NULL)
    {
#ifdef UNIX
	if (mch_stat((char *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_mode;
	    st_old_valid = TRUE;
	}
#else
	perm = mch_getperm(buf->b_ffname);
	if (perm < 0)
	    perm = 0600;
#endif
    }

    /* strip any s-bit and executable bit */
    perm = perm & 0666;

    /* If the undo file already exists, verify that it actually is an undo
     * file, and delete it. */
    if (mch_getperm(file_name) >= 0)
    {
	if (name == NULL || !forceit)
	{
	    /* Check we can read it and it's an undo file. */
	    fd = mch_open((char *)file_name, O_RDONLY|O_EXTRA, 0);
	    if (fd < 0)
	    {
		if (name != NULL || p_verbose > 0)
		{
		    if (name == NULL)
			verbose_enter();
		    smsg((char_u *)
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    if (name == NULL)
			verbose_leave();
		}
		goto theend;
	    }
	    else
	    {
		char_u	mbuf[UF_START_MAGIC_LEN];
		int	len;

		len = read_eintr(fd, mbuf, UF_START_MAGIC_LEN);
		close(fd);
		if (len < UF_START_MAGIC_LEN
		      || memcmp(mbuf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
		{
		    if (name != NULL || p_verbose > 0)
		    {
			if (name == NULL)
			    verbose_enter();
			smsg((char_u *)
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			if (name == NULL)
			    verbose_leave();
		    }
		    goto theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    /* If there is no undo information at all, quit here after deleting any
     * existing undo file. */
    if (buf->b_u_numhead == 0 && buf->b_u_line_ptr == NULL)
    {
	if (p_verbose > 0)
	    verb_msg((char_u *)_("Skipping undo file write, nothing to undo"));
	goto theend;
    }

    fd = mch_open((char *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    if (fd < 0)
    {
	EMSG2(_(e_not_open), file_name);
	goto theend;
    }
    (void)mch_setperm(file_name, perm);
    if (p_verbose > 0)
    {
	verbose_enter();
	smsg((char_u *)_("Writing undo file: %s"), file_name);
	verbose_leave();
    }

#ifdef U_DEBUG
    /* Check there is no problem in undo info before writing. */
    u_check(FALSE);
#endif

#ifdef UNIX
    /*
     * Try to set the group of the undo file same as the original file. If
     * this fails, set the protection bits for the group same as the
     * protection bits for others.
     */
    if (st_old_valid
	    && mch_stat((char *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  /* sequent-ptx lacks fchown() */
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
# endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
# if defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    if (buf->b_ffname != NULL)
	mch_copy_sec(buf->b_ffname, file_name);
# endif
#endif

    fp = fdopen(fd, "w");
    if (fp == NULL)
    {
	EMSG2(_(e_not_open), file_name);
	close(fd);
	mch_remove(file_name);
	goto theend;
    }

    /* Undo must be synced. */
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    if (serialize_header(&bi, hash) == FAIL)
	goto write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    while (uhp != NULL)
    {
	/* Serialize current UHP if we haven't seen it */
	if (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
#ifdef U_DEBUG
	    ++headers_written;
#endif
	    if (serialize_uhp(&bi, uhp) == FAIL)
		goto write_error;
	}

	/* Now walk through the tree - algorithm from undo_time(). */
	if (uhp->uh_prev.ptr != NULL && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	else if (uhp->uh_alt_next.ptr != NULL
				     && uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;
	else if (uhp->uh_next.ptr != NULL && uhp->uh_alt_prev.ptr == NULL
					 && uhp->uh_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_next.ptr;
	else if (uhp->uh_alt_prev.ptr != NULL)
	    uhp = uhp->uh_alt_prev.ptr;
	else
	    uhp = uhp->uh_next.ptr;
    }

    if (undo_write_bytes(&bi, (long_u)UF_HEADER_END_MAGIC, 2) == OK)
	write_ok = TRUE;
#ifdef U_DEBUG
    if (headers_written != buf->b_u_numhead)
    {
	EMSGN("Written %ld headers, ...", headers_written);
	EMSGN("... but numhead is %ld", buf->b_u_numhead);
    }
#endif

#ifdef FEAT_CRYPT
    if (bi.bi_state != NULL && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
#endif

write_error:
    fclose(fp);
    if (!write_ok)
	EMSG2(_("E829: write error in undo file: %s"), file_name);

#if defined(MACOS_CLASSIC) || defined(WIN3264)
    /* Copy file attributes; for systems where this can only be done after
     * closing the file. */
    if (buf->b_ffname != NULL)
	(void)mch_copy_file_attribute(buf->b_ffname, file_name);
#endif
#ifdef HAVE_ACL
    if (buf->b_ffname != NULL)
    {
	vim_acl_T	    acl;

	/* For systems that support ACL: get the ACL from the original file. */
	acl = mch_get_acl(buf->b_ffname);
	mch_set_acl(file_name, acl);
	mch_free_acl(acl);
    }
#endif

theend:
#ifdef FEAT_CRYPT
    if (bi.bi_state != NULL)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
#endif
    if (file_name != name)
	vim_free(file_name);
}