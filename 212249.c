Variant HHVM_FUNCTION(setlocale,
                      int category,
                      const Variant& locale,
                      const Array& _argv /* = null_array */) {
  Array argv = _argv;
  if (locale.isArray()) {
    if (!argv.empty()) raise_invalid_argument_warning("locale: not string)");
    argv = locale; // ignore _argv
  }

  for (int i = -1; i < argv.size(); i++) {
    String slocale;
    if (i == -1) {
      if (locale.isArray()) continue;
      slocale = locale.toString();
    } else {
      slocale = argv[i].toString();
    }

    const char *loc = slocale.c_str();
    if (slocale.size() >= 255) {
      raise_invalid_argument_warning("locale name is too long: %s", loc);
      return false;
    }
    if (strcmp("0", loc) == 0) {
      loc = nullptr;
    }
    {
      Lock lock(s_mutex);
      const char *retval = setlocale(category, loc);
      if (retval) {
        return String(retval, CopyString);
      }
    }
  }
  return false;
}