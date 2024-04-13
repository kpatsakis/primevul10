do_ed_script (char const *inname, char const *outname,
	      bool *outname_needs_removal, FILE *ofp)
{
    static char const editor_program[] = EDITOR_PROGRAM;

    file_offset beginning_of_this_line;
    size_t chars_read;
    FILE *tmpfp = 0;
    int tmpfd = -1; /* placate gcc's -Wmaybe-uninitialized */
    int exclusive = *outname_needs_removal ? 0 : O_EXCL;
    char const **ed_argv;
    int stdin_dup, status;


    if (! dry_run && ! skip_rest_of_patch)
      {
	/* Write ed script to a temporary file.  This causes ed to abort on
	   invalid commands such as when line numbers or ranges exceed the
	   number of available lines.  When ed reads from a pipe, it rejects
	   invalid commands and treats the next line as a new command, which
	   can lead to arbitrary command execution.  */

	tmpfd = make_tempfile (&TMPEDNAME, 'e', NULL, O_RDWR | O_BINARY, 0);
	if (tmpfd == -1)
	  pfatal ("Can't create temporary file %s", quotearg (TMPEDNAME));
	TMPEDNAME_needs_removal = true;
	tmpfp = fdopen (tmpfd, "w+b");
	if (! tmpfp)
	  pfatal ("Can't open stream for file %s", quotearg (TMPEDNAME));
      }

    for (;;) {
	char ed_command_letter;
	beginning_of_this_line = file_tell (pfp);
	chars_read = get_line ();
	if (! chars_read) {
	    next_intuit_at(beginning_of_this_line,p_input_line);
	    break;
	}
	ed_command_letter = get_ed_command_letter (buf);
	if (ed_command_letter) {
	    if (tmpfp)
		if (fwrite (buf, sizeof *buf, chars_read, tmpfp) < chars_read)
		    write_fatal ();
	    if (ed_command_letter != 'd' && ed_command_letter != 's') {
	        p_pass_comments_through = true;
		while ((chars_read = get_line ()) != 0) {
		    if (tmpfp)
			if (fwrite (buf, sizeof *buf, chars_read, tmpfp)
			    < chars_read)
			    write_fatal ();
		    if (chars_read == 2  &&  strEQ (buf, ".\n"))
			break;
		}
		p_pass_comments_through = false;
	    }
	}
	else {
	    next_intuit_at(beginning_of_this_line,p_input_line);
	    break;
	}
    }
    if (dry_run || skip_rest_of_patch)
      return;
    if (fwrite ("w\nq\n", sizeof (char), (size_t) 4, tmpfp) < (size_t) 4
	|| fflush (tmpfp) != 0)
      write_fatal ();

    if (lseek (tmpfd, 0, SEEK_SET) == -1)
      pfatal ("Can't rewind to the beginning of file %s", quotearg (TMPEDNAME));

    if (inerrno != ENOENT)
      {
	*outname_needs_removal = true;
	copy_file (inname, outname, 0, exclusive, instat.st_mode, true);
      }
    fflush (stdout);

    if ((stdin_dup = dup (0)) == -1
	|| dup2 (tmpfd, 0) == -1)
      pfatal ("Failed to duplicate standard input");
    assert (outname[0] != '!' && outname[0] != '-');
    ed_argv = alloca (4 * sizeof * ed_argv);
    ed_argv[0] = editor_program;
    ed_argv[1] = "-";
    ed_argv[2] = outname;
    ed_argv[3] = (char  *) NULL;
    status = execute (editor_program, editor_program, (char **)ed_argv,
		      false, false, false, false, true, false, NULL);
    if (status)
      fatal ("%s FAILED", editor_program);
    if (dup2 (stdin_dup, 0) == -1
	|| close (stdin_dup) == -1)
      pfatal ("Failed to duplicate standard input");

    fclose (tmpfp);

    if (ofp)
      {
	FILE *ifp = fopen (outname, binary_transput ? "rb" : "r");
	int c;
	if (!ifp)
	  pfatal ("can't open '%s'", outname);
	while ((c = getc (ifp)) != EOF)
	  if (putc (c, ofp) == EOF)
	    write_fatal ();
	if (ferror (ifp) || fclose (ifp) != 0)
	  read_fatal ();
      }
}