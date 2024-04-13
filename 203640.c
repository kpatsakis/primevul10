wgetrc_file_name (void)
{
  char *file = wgetrc_env_file_name ();
  if (file && *file)
    return file;

  file = wgetrc_user_file_name ();

#ifdef WINDOWS
  /* Under Windows, if we still haven't found .wgetrc, look for the file
     `wget.ini' in the directory where `wget.exe' resides; we do this for
     backward compatibility with previous versions of Wget.
     SYSTEM_WGETRC should not be defined under WINDOWS.  */
  if (!file)
    {
      char *home = home_dir ();
      xfree_null (file);
      file = NULL;
      home = ws_mypath ();
      if (home)
        {
          file = aprintf ("%s/wget.ini", home);
          if (!file_exists_p (file))
            {
              xfree (file);
              file = NULL;
            }
          xfree (home);
        }
    }
#endif /* WINDOWS */

  return file;
}