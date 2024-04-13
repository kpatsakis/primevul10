bool str_contains_any_of(const String& haystack, const String& char_list) {
  return strpbrk_impl<true>(haystack, char_list).toBooleanVal();
}