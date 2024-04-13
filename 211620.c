Variant preg_replace_impl(const Variant& pattern, const Variant& replacement,
                          const Variant& subject, int limit, int64_t* count,
                          bool is_callable, bool is_filter) {
  assertx(!(is_callable && is_filter));
  if (!is_callable &&
      replacement.isArray() && !pattern.isArray()) {
    raise_warning("Parameter mismatch, pattern is a string while "
                    "replacement is an array");
    return preg_return_internal_error(false);
  }

  int replace_count = 0;
  if (!isContainer(subject)) {
    auto ret = php_replace_in_subject(pattern, replacement, subject.toString(),
                                      limit, is_callable, &replace_count);

    if (ret.isNull()) return ret; // php_replace_in_subject already set error
    assertx(ret.isString());
    if (count) *count = replace_count;
    if (is_filter && replace_count == 0) {
      return preg_return_internal_error(init_null());
    }
    return preg_return_no_error(std::move(ret));
  }

  Array return_value = Array::CreateDArray();
  Array arrSubject = subject.toDArray();
  for (ArrayIter iter(arrSubject); iter; ++iter) {
    auto old_replace_count = replace_count;
    String subject_entry = iter.second().toString();
    auto ret = php_replace_in_subject(pattern, replacement, subject_entry,
                                      limit, is_callable, &replace_count);

    if (ret.isString() && (!is_filter || replace_count > old_replace_count)) {
      return_value.set(iter.first(), ret.asStrRef());
    }
  }
  if (count) *count = replace_count;
  return preg_return_no_error(std::move(return_value));
}