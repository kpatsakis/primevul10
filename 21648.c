bash_servicename_completion_function (text, state)
     const char *text;
     int state;
{
#if defined (__WIN32__) || defined (__OPENNT) || !defined (HAVE_GETSERVENT)
  return ((char *)NULL);
#else
  static char *sname = (char *)NULL;
  static struct servent *srvent;
  static int snamelen;
  char *value;
  char **alist, *aentry;
  int afound;

  if (state == 0)
    {
      FREE (sname);

      sname = savestring (text);
      snamelen = strlen (sname);
      setservent (0);
    }

  while (srvent = getservent ())
    {
      afound = 0;
      if (snamelen == 0 || (STREQN (sname, srvent->s_name, snamelen)))
	break;
      /* Not primary, check aliases */
      for (alist = srvent->s_aliases; *alist; alist++)
	{
	  aentry = *alist;
	  if (STREQN (sname, aentry, snamelen))
	    {
	      afound = 1;
	      break;
	    }
	}

      if (afound)
	break;
    }

  if (srvent == 0)
    {
      endservent ();
      return ((char *)NULL);
    }

  value = afound ? savestring (aentry) : savestring (srvent->s_name);
  return value;
#endif
}