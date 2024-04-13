void vimrc_found(char_u *fname, char_u *envname)
{
  char_u      *p;

  if (fname != NULL) {
    p = (char_u *)vim_getenv((char *)envname);
    if (p == NULL) {
      /* Set $MYVIMRC to the first vimrc file found. */
      p = (char_u *)FullName_save((char *)fname, FALSE);
      if (p != NULL) {
        vim_setenv((char *)envname, (char *)p);
        xfree(p);
      }
    } else {
      xfree(p);
    }
  }
}