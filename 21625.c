extglob_skipname (pat, dname, flags)
     char *pat, *dname;
     int flags;
{
  char *pp, *pe, *t, *se;
  int n, r, negate, wild, nullpat;

  negate = *pat == '!';
  wild = *pat == '*' || *pat == '?';
  pp = pat + 2;
  se = pp + strlen (pp) - 1;		/* end of string */
  pe = glob_patscan (pp, se, 0);	/* end of extglob pattern (( */
  /* we should check for invalid extglob pattern here */
  if (pe == 0)
    return 0;

  /* if pe != se we have more of the pattern at the end of the extglob
     pattern. Check the easy case first ( */
  if (pe == se && *pe == ')' && (t = strchr (pp, '|')) == 0)
    {
      *pe = '\0';
#if defined (HANDLE_MULTIBYTE)
      r = mbskipname (pp, dname, flags);
#else
      r = skipname (pp, dname, flags);	/*(*/
#endif
      *pe = ')';
      if (wild && pe[1])	/* if we can match zero instances, check further */
        return (skipname (pe+1, dname, flags));
      return r;
    }

  /* Is the extglob pattern between the parens the null pattern?  The null
     pattern can match nothing, so should we check any remaining portion of
     the pattern? */
  nullpat = pe >= (pat + 2) && pe[-2] == '(' && pe[-1] == ')';

  /* check every subpattern */
  while (t = glob_patscan (pp, pe, '|'))
    {
      n = t[-1];	/* ( */
      if (extglob_pattern_p (pp) && n == ')')
	t[-1] = n;	/* no-op for now */
      else
	t[-1] = '\0';
#if defined (HANDLE_MULTIBYTE)
      r = mbskipname (pp, dname, flags);
#else
      r = skipname (pp, dname, flags);
#endif
      t[-1] = n;
      if (r == 0)	/* if any pattern says not skip, we don't skip */
        return r;
      pp = t;
    }	/*(*/

  /* glob_patscan might find end of string */
  if (pp == se)
    return r;

  /* but if it doesn't then we didn't match a leading dot */
  if (wild && *pe)	/* if we can match zero instances, check further */
    return (skipname (pe, dname, flags));
  return 1;
}