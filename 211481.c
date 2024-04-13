TypedValue HHVM_FUNCTION(substr, StringArg str, int start, int length) {
  auto const size = str.get()->size();
  if (!string_substr_check(size, start, length)) {
    if (RuntimeOption::PHP7_Substr && size == start) {
      return make_tv<KindOfPersistentString>(empty_string_ref.get());
    } else {
      return make_tv<KindOfBoolean>(false);
    }
  }
  return tvReturn(String::attach(str.get()->substr(start, length)));
}