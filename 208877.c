int ascii_compare(const void *s1, const void *s2) {
   return stricmp(*(char **) s1, *(char **) s2);
}