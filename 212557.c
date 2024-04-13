static void json_create_zval(Variant &z, UncheckedBuffer &buf, DataType type,
                             int64_t options) {
  switch (type) {
    case KindOfBoolean:
      z = (buf.data() && (*buf.data() == 't'));
      return;

    case KindOfInt64: {
      bool bigint = false;
      const char *p = buf.data();
      assertx(p);
      if (p == NULL) {
        z = int64_t(0);
        return;
      }

      bool neg = *buf.data() == '-';

      int len = buf.size();
      if (neg) len--;
      if (len >= MAX_LENGTH_OF_LONG - 1) {
        if (len == MAX_LENGTH_OF_LONG - 1) {
          int cmp = strcmp(p + (neg ? 1 : 0), long_min_digits);
          if (!(cmp < 0 || (cmp == 0 && neg))) {
            bigint = true;
          }
        } else {
          bigint = true;
        }
      }

      if (bigint) {
        if (!(options & k_JSON_BIGINT_AS_STRING)) {
          // See KindOfDouble (below)
          z = to_double(buf);
        } else {
          z = copy_and_clear(buf);
        }
      } else {
        z = fast_strtoll_base10(buf.data());
      }
      return;
    }

    case KindOfDouble:
      // Use zend_strtod() instead of strtod() here since JSON specifies using
      // a '.' for decimal separators regardless of locale.
      z = to_double(buf);
      return;

    case KindOfString:
      z = copy_and_clear(buf);
      return;

    case KindOfUninit:
    case KindOfNull:
    case KindOfPersistentString:
    case KindOfPersistentDArray:
    case KindOfDArray:
    case KindOfPersistentVArray:
    case KindOfVArray:
    case KindOfPersistentArray:
    case KindOfArray:
    case KindOfPersistentVec:
    case KindOfVec:
    case KindOfPersistentDict:
    case KindOfDict:
    case KindOfPersistentKeyset:
    case KindOfKeyset:
    case KindOfObject:
    case KindOfResource:
    case KindOfFunc:
    case KindOfClass:
    case KindOfClsMeth:
    case KindOfRecord:
      z = uninit_null();
      return;
  }
  not_reached();
}