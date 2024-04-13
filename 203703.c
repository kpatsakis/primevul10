there_is_another_patch (bool need_header, mode_t *file_type)
{
    if (p_base != 0 && p_base >= p_filesize) {
	if (verbosity == VERBOSE)
	    say ("done\n");
	return false;
    }
    if (verbosity == VERBOSE)
	say ("Hmm...");
    diff_type = intuit_diff_type (need_header, file_type);
    if (diff_type == NO_DIFF) {
	if (verbosity == VERBOSE)
	  say (p_base
	       ? "  Ignoring the trailing garbage.\ndone\n"
	       : "  I can't seem to find a patch in there anywhere.\n");
	if (! p_base && p_filesize)
	  fatal ("Only garbage was found in the patch input.");
	return false;
    }
    if (skip_rest_of_patch)
      {
	Fseek (pfp, p_start, SEEK_SET);
	p_input_line = p_sline - 1;
	return true;
      }
    if (verbosity == VERBOSE)
	say ("  %sooks like %s to me...\n",
	    (p_base == 0 ? "L" : "The next patch l"),
	    diff_type == UNI_DIFF ? "a unified diff" :
	    diff_type == CONTEXT_DIFF ? "a context diff" :
	    diff_type == NEW_CONTEXT_DIFF ? "a new-style context diff" :
	    diff_type == NORMAL_DIFF ? "a normal diff" :
	    diff_type == GIT_BINARY_DIFF ? "a git binary diff" :
	    "an ed script" );

    if (no_strip_trailing_cr)
      p_strip_trailing_cr = false;

    if (verbosity != SILENT)
      {
	if (p_indent)
	  say ("(Patch is indented %lu space%s.)\n",
	       (unsigned long int) p_indent, p_indent==1?"":"s");
	if (p_strip_trailing_cr)
	  say ("(Stripping trailing CRs from patch; use --binary to disable.)\n");
	if (! inname)
	  {
	    char numbuf[LINENUM_LENGTH_BOUND + 1];
	    say ("can't find file to patch at input line %s\n",
		 format_linenum (numbuf, p_sline));
	    if (diff_type != ED_DIFF && diff_type != NORMAL_DIFF)
	      say (strippath == -1
		   ? "Perhaps you should have used the -p or --strip option?\n"
		   : "Perhaps you used the wrong -p or --strip option?\n");
	  }
      }

    skip_to(p_start,p_sline);
    while (!inname) {
	char *t;
	if (force | batch) {
	    say ("No file to patch.  Skipping patch.\n");
	    skip_rest_of_patch = true;
	    return true;
	}
	ask ("File to patch: ");
	t = buf + strlen (buf);
	if (t > buf + 1 && *(t - 1) == '\n')
	  {
	    inname = xmemdup0 (buf, t - buf - 1);
	    inerrno = stat_file (inname, &instat);
	    if (inerrno)
	      {
		perror (inname);
		fflush (stderr);
		free (inname);
		inname = 0;
	      }
	    else
	      invc = -1;
	  }
	if (!inname) {
	    ask ("Skip this patch? [y] ");
	    if (*buf != 'n') {
		if (verbosity != SILENT)
		    say ("Skipping patch.\n");
		skip_rest_of_patch = true;
		return true;
	    }
	}
    }
    return true;
}