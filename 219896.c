ex_undolist(exarg_T *eap UNUSED)
{
    garray_T	ga;
    u_header_T	*uhp;
    int		mark;
    int		nomark;
    int		changes = 1;
    int		i;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, (int)sizeof(char *), 20);

    uhp = curbuf->b_u_oldhead;
    while (uhp != NULL)
    {
	if (uhp->uh_prev.ptr == NULL && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    if (ga_grow(&ga, 1) == FAIL)
		break;
	    vim_snprintf((char *)IObuff, IOSIZE, "%6ld %7ld  ",
							uhp->uh_seq, changes);
	    u_add_time(IObuff + STRLEN(IObuff), IOSIZE - STRLEN(IObuff),
								uhp->uh_time);
	    if (uhp->uh_save_nr > 0)
	    {
		while (STRLEN(IObuff) < 33)
		    STRCAT(IObuff, " ");
		vim_snprintf_add((char *)IObuff, IOSIZE,
						   "  %3ld", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_data))[ga.ga_len++] = vim_strsave(IObuff);
	}

	uhp->uh_walk = mark;

	/* go down in the tree if we haven't been there */
	if (uhp->uh_prev.ptr != NULL && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	/* go to alternate branch if we haven't been there */
	else if (uhp->uh_alt_next.ptr != NULL
		&& uhp->uh_alt_next.ptr->uh_walk != nomark
		&& uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;

	/* go up in the tree if we haven't been there and we are at the
	 * start of alternate branches */
	else if (uhp->uh_next.ptr != NULL && uhp->uh_alt_prev.ptr == NULL
		&& uhp->uh_next.ptr->uh_walk != nomark
		&& uhp->uh_next.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_next.ptr;
	    --changes;
	}

	else
	{
	    /* need to backtrack; mark this node as done */
	    uhp->uh_walk = nomark;
	    if (uhp->uh_alt_prev.ptr != NULL)
		uhp = uhp->uh_alt_prev.ptr;
	    else
	    {
		uhp = uhp->uh_next.ptr;
		--changes;
	    }
	}
    }

    if (ga.ga_len == 0)
	MSG(_("Nothing to undo"));
    else
    {
	sort_strings((char_u **)ga.ga_data, ga.ga_len);

	msg_start();
	msg_puts_attr((char_u *)_("number changes  when               saved"),
							      hl_attr(HLF_T));
	for (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    msg_putchar('\n');
	    if (got_int)
		break;
	    msg_puts(((char_u **)ga.ga_data)[i]);
	}
	msg_end();

	ga_clear_strings(&ga);
    }
}