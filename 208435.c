void set_fileformat(int eol_style, int opt_flags)
{
  char *p = NULL;

  switch (eol_style) {
      case EOL_UNIX:
          p = FF_UNIX;
          break;
      case EOL_MAC:
          p = FF_MAC;
          break;
      case EOL_DOS:
          p = FF_DOS;
          break;
  }

  // p is NULL if "eol_style" is EOL_UNKNOWN.
  if (p != NULL) {
    set_string_option_direct((char_u *)"ff",
                             -1,
                             (char_u *)p,
                             OPT_FREE | opt_flags,
                             0);
  }

  // This may cause the buffer to become (un)modified.
  check_status(curbuf);
  redraw_tabline = true;
  need_maketitle = true;  // Set window title later.
}