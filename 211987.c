int64_t HHVM_FUNCTION(similar_text,
                      const String& first,
                      const String& second,
                      double& percent) {
  int ret = string_similar_text(first.data(), first.size(),
                                second.data(), second.size(), &percent);
  return ret;
}