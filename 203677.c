wgetrc_user_file_name (void)
{
  char *home;
  char *file = NULL;
  /* If that failed, try $HOME/.wgetrc (or equivalent).  */

#ifdef __VMS
  file = "SYS$LOGIN:.wgetrc";
#else /* def __VMS */
  home = home_dir ();
  if (home)
    file = aprintf ("%s/.wgetrc", home);
  xfree_null (home);
#endif /* def __VMS [else] */

  if (!file)
    return NULL;
  if (!file_exists_p (file))
    {
      xfree (file);
      return NULL;
    }
  return file;
}