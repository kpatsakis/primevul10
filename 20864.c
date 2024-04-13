msdosify (const char *file_name)
{
  static char dos_name[PATH_MAX];
  static const char illegal_chars_dos[] = ".+, ;=[]|<>\\\":?*";
  static const char *illegal_chars_w95 = &illegal_chars_dos[8];
  int idx, dot_idx;
  const char *s = file_name;
  char *d = dos_name;
  const char *illegal_aliens = illegal_chars_dos;
  size_t len = sizeof (illegal_chars_dos) - 1;
  int lfn = 0;

#ifdef DJGPP
  /* Support for Windows 9X VFAT systems, when available (djgpp only). */
  if (_use_lfn (file_name))
    lfn = 1;
#endif
  if (lfn) {
    illegal_aliens = illegal_chars_w95;
    len -= (illegal_chars_w95 - illegal_chars_dos);
  }

  /* Get past the drive letter, if any. */
  if (s[0] >= 'A' && s[0] <= 'z' && s[1] == ':') {
    *d++ = *s++;
    *d++ = *s++;
  }

  for (idx = 0, dot_idx = -1; *s; s++, d++) {
    if (memchr (illegal_aliens, *s, len)) {
      /* Dots are special: DOS doesn't allow them as the leading character,
         and a file name cannot have more than a single dot.  We leave the
         first non-leading dot alone, unless it comes too close to the
         beginning of the name: we want sh.lex.c to become sh_lex.c, not
         sh.lex-c.  */
      if (*s == '.') {
        if (idx == 0 && (s[1] == '/' || (s[1] == '.' && s[2] == '/'))) {
          /* Copy "./" and "../" verbatim.  */
          *d++ = *s++;
          if (*s == '.')
            *d++ = *s++;
          *d = *s;
        }
        else if (idx == 0)
          *d = '_';
        else if (dot_idx >= 0) {
          if (dot_idx < 5) { /* 5 is a heuristic ad-hoc'ery */
            d[dot_idx - idx] = '_'; /* replace previous dot */
            *d = '.';
          }
          else
            *d = '-';
        }
        else
          *d = '.';

        if (*s == '.')
          dot_idx = idx;
      }
      else if (*s == '+' && s[1] == '+') {
        if (idx - 2 == dot_idx) { /* .c++, .h++ etc. */
          *d++ = 'x';
          *d   = 'x';
        }
        else {
          /* libg++ etc.  */
          memcpy (d, "plus", 4);
          d += 3;
        }
        s++;
        idx++;
      }
      else
        *d = '_';
    }
    else
      *d = *s;
    if (*s == '/') {
      idx = 0;
      dot_idx = -1;
    }
    else
      idx++;
  }

  *d = '\0';
  return dos_name;
}