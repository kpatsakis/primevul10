TypedValue HHVM_FUNCTION(strpbrk,
                         const String& haystack,
                         const String& char_list) {
  return tvReturn(strpbrk_impl<false>(haystack, char_list));
}