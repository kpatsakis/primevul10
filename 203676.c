home_dir (void)
{
  static char *buf = NULL;
  static char *home, *ret;

  if (!home)
    {
      home = getenv ("HOME");
      if (!home)
        {
#if defined(MSDOS)
          int len;

          /* Under MSDOS, if $HOME isn't defined, use the directory where
             `wget.exe' resides.  */
          const char *_w32_get_argv0 (void); /* in libwatt.a/pcconfig.c */
          char *p;

          buff = _w32_get_argv0 ();

          p = strrchr (buf, '/');            /* djgpp */
          if (!p)
            p = strrchr (buf, '\\');          /* others */
          assert (p);

          len = p - buff + 1;
          buff = strdup (_w32_get_argv0 ());

          home = buf;
#elif !defined(WINDOWS)
          /* If HOME is not defined, try getting it from the password
             file.  */
          struct passwd *pwd = getpwuid (getuid ());
          if (!pwd || !pwd->pw_dir)
            return NULL;
          home = pwd->pw_dir;
#else  /* !WINDOWS */
          /* Under Windows, if $HOME isn't defined, use the directory where
             `wget.exe' resides.  */
          home = ws_mypath ();
#endif /* WINDOWS */
        }
    }

  ret = home ? xstrdup (home) : NULL;
  free (buf);

  return ret;
}