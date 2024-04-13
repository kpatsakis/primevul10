skipname (pat, dname, flags)
     char *pat;
     char *dname;
     int flags;
{
#if EXTENDED_GLOB
  if (extglob_pattern_p (pat))		/* XXX */
    return (extglob_skipname (pat, dname, flags));
#endif

  if (glob_always_skip_dot_and_dotdot && DOT_OR_DOTDOT (dname))
    return 1;

  /* If a leading dot need not be explicitly matched, and the pattern
     doesn't start with a `.', don't match `.' or `..' */
  if (noglob_dot_filenames == 0 && pat[0] != '.' &&
	(pat[0] != '\\' || pat[1] != '.') &&
	DOT_OR_DOTDOT (dname))
    return 1;

  /* If a dot must be explicitly matched, check to see if they do. */
  else if (noglob_dot_filenames && dname[0] == '.' && pat[0] != '.' &&
	(pat[0] != '\\' || pat[1] != '.'))
    return 1;

  return 0;
}