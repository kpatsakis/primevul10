const char *git_revision() {
   const char *p = _git_revision;
   if (strrchr(p, '-'))
      p = strrchr(p, '-') + 2;
   return p;
}