static Variant php_replace_in_subject(const Variant& regex, const Variant& replace,
                                      String subject, int limit, bool callable,
                                      int* replace_count) {
  if (!regex.isArray()) {
    return php_pcre_replace(regex.toString(), subject, replace, callable,
                            limit, replace_count);
  }

  if (callable || !replace.isArray()) {
    Array arr = regex.toDArray();
    for (ArrayIter iterRegex(arr); iterRegex; ++iterRegex) {
      String regex_entry = iterRegex.second().toString();
      auto ret = php_pcre_replace(regex_entry, subject, replace, callable,
                                  limit, replace_count);
      if (!ret.isString()) {
        assertx(ret.isNull());
        return ret; // php_pcre_replace already set error
      }
      subject = ret.asStrRef();
      assertx(!subject.isNull());
    }
    return preg_return_no_error(std::move(subject));
  }

  Array arrReplace = replace.toDArray();
  Array arrRegex = regex.toDArray();
  ArrayIter iterReplace(arrReplace);
  for (ArrayIter iterRegex(arrRegex); iterRegex; ++iterRegex) {
    String regex_entry = iterRegex.second().toString();
    Variant replace_value;
    if (iterReplace) {
      replace_value = iterReplace.second();
      ++iterReplace;
    }

    auto ret = php_pcre_replace(regex_entry, subject, replace_value, callable,
                                limit, replace_count);
    if (!ret.isString()) {
      assertx(ret.isNull());
      return ret; // php_pcre_replace already set error
    }
    subject = ret.asStrRef();
    assertx(!subject.isNull());
  }
  return preg_return_no_error(std::move(subject));
}