wskipname (pat, dname, flags)
     wchar_t *pat, *dname;
     int flags;
{
  if (glob_always_skip_dot_and_dotdot && WDOT_OR_DOTDOT (dname))
    return 1;

  /* If a leading dot need not be explicitly matched, and the
     pattern doesn't start with a `.', don't match `.' or `..' */
  if (noglob_dot_filenames == 0 && pat[0] != L'.' &&
	(pat[0] != L'\\' || pat[1] != L'.') &&
	WDOT_OR_DOTDOT (dname))
    return 1;

  /* If a leading dot must be explicitly matched, check to see if the
     pattern and dirname both have one. */
 else if (noglob_dot_filenames && dname[0] == L'.' &&
	pat[0] != L'.' &&
	   (pat[0] != L'\\' || pat[1] != L'.'))
    return 1;

  return 0;
}