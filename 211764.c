String HHVM_FUNCTION(strip_tags,
                     const String& str,
                     const Variant& allowable_tags /* = "" */) {
  return StringUtil::StripHTMLTags(str, allowable_tags.toString());
}