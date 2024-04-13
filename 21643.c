restore_tilde (val, directory_part)
     char *val, *directory_part;
{
  int l, vl, dl2, xl;
  char *dh2, *expdir, *ret, *v;

  vl = strlen (val);

  /* We need to duplicate the expansions readline performs on the directory
     portion before passing it to our completion function. */
  dh2 = directory_part ? bash_dequote_filename (directory_part, 0) : 0;
  bash_directory_expansion (&dh2);
  dl2 = strlen (dh2);

  expdir = bash_tilde_expand (directory_part, 0);
  xl = strlen (expdir);
  if (*directory_part == '~' && STREQ (directory_part, expdir))
    {
      /* tilde expansion failed, so what should we return? we use what the
	 user typed. */
      v = mbschr (val, '/');
      vl = STRLEN (v);
      ret = (char *)xmalloc (xl + vl + 2);
      strcpy (ret, directory_part);
      if (v && *v)
	strcpy (ret + xl, v);

      free (dh2);
      free (expdir);

      return ret;
    }
  free (expdir);

  /*
     dh2 = unexpanded but dequoted tilde-prefix
     dl2 = length of tilde-prefix
     expdir = tilde-expanded tilde-prefix
     xl = length of expanded tilde-prefix
     l = length of remainder after tilde-prefix
  */
  l = (vl - xl) + 1;
  if (l <= 0)
    {
      free (dh2);
      return (savestring (val));		/* XXX - just punt */
    }

  ret = (char *)xmalloc (dl2 + 2 + l);
  strcpy (ret, dh2);
  strcpy (ret + dl2, val + xl);

  free (dh2);
  return (ret);
}