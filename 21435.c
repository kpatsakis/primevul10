size_add_wrapv (size_t a, size_t b, size_t *r)
{
#if 5 <= __GNUC__ && !defined __ICC
  return __builtin_add_overflow (a, b, r);
#else
  *r = a + b;
  return *r < a;
#endif
}