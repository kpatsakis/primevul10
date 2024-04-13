free_charset (re_charset_t *cset)
{
  re_free (cset->mbchars);
# ifdef _LIBC
  re_free (cset->coll_syms);
  re_free (cset->equiv_classes);
  re_free (cset->range_starts);
  re_free (cset->range_ends);
# endif
  re_free (cset->char_classes);
  re_free (cset);
}