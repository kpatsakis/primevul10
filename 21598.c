mbskipname (pat, dname, flags)
     char *pat, *dname;
     int flags;
{
  int ret, ext;
  wchar_t *pat_wc, *dn_wc;
  size_t pat_n, dn_n;

  if (mbsmbchar (dname) == 0 && mbsmbchar (pat) == 0)
    return (skipname (pat, dname, flags));

  ext = 0;
#if EXTENDED_GLOB
  ext = extglob_pattern_p (pat);
#endif

  pat_wc = dn_wc = (wchar_t *)NULL;

  pat_n = xdupmbstowcs (&pat_wc, NULL, pat);
  if (pat_n != (size_t)-1)
    dn_n = xdupmbstowcs (&dn_wc, NULL, dname);

  ret = 0;
  if (pat_n != (size_t)-1 && dn_n !=(size_t)-1)
    ret = ext ? wextglob_skipname (pat_wc, dn_wc, flags) : wskipname (pat_wc, dn_wc, flags);
  else
    ret = skipname (pat, dname, flags);

  FREE (pat_wc);
  FREE (dn_wc);

  return ret;
}