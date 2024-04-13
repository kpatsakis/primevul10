static Variant substr_replace(const Variant& str, const Variant& replacement,
                              const Variant& start, const Variant& length) {
  if (!str.isArray()) {
    String repl;
    if (replacement.isArray()) {
      repl = replacement.asCArrRef()[0].toString();
    } else {
      repl = replacement.toString();
    }
    if (start.isArray()) {
      if (!length.isArray()) {
        raise_invalid_argument_warning("start and length should be of same type - "
                               "numerical or array");
        return str;
      }
      Array startArr = start.toArray();
      Array lengthArr = length.toArray();
      if (startArr.size() != lengthArr.size()) {
        raise_invalid_argument_warning("start and length: (different item count)");
        return str;
      }
      raise_invalid_argument_warning("start and length as arrays not implemented");
      return str;
    }
    return string_replace(str.toString(), start.toInt32(), length.toInt32(),
                          repl);
  }

  // 'start' and 'length' can be arrays (in which case we step through them in
  // sync with stepping through 'str'), or not arrays, in which case we convert
  // them to ints and always use those.
  Array ret;
  Array strArr = str.toArray();
  folly::Optional<int> opStart;
  folly::Optional<int> opLength;
  if (!start.isArray()) {
    opStart = start.toInt32();
  }
  if (!length.isArray()) {
    opLength = length.toInt32();
  }

  Array startArr = start.toArray();
  Array lengthArr = length.toArray();
  ArrayIter startIter(startArr);
  ArrayIter lengthIter(lengthArr);

  if (replacement.isArray()) {
    Array replArr = replacement.toArray();
    ArrayIter replIter(replArr);
    for (ArrayIter iter(strArr); iter; ++iter) {
      auto str = iter.second().toString();
      // If 'start' or 'length' are arrays and we've gone past the end, default
      // to 0 for start and the length of the input string for length.
      int nStart =
        (opStart.has_value()
         ? opStart.value()
         : (startIter ? startIter.second().toInt32() : 0));
      int nLength =
        (opLength.has_value()
         ? opLength.value()
         : (lengthIter ? lengthIter.second().toInt32() : str.length()));
      if (startIter) ++startIter;
      if (lengthIter) ++lengthIter;

      String repl;
      if (replIter) {
        repl = replIter.second().toString();
        ++replIter;
      } else {
        repl = empty_string();
      }
      auto s2 = string_replace(str, nStart, nLength, repl);
      ret.append(s2);
    }
  } else {
    String repl = replacement.toString();
    for (ArrayIter iter(strArr); iter; ++iter) {
      auto str = iter.second().toString();
      int nStart =
        (opStart.has_value()
         ? opStart.value()
         : (startIter ? startIter.second().toInt32() : 0));
      int nLength =
        (opLength.has_value()
         ? opLength.value()
         : (lengthIter ? lengthIter.second().toInt32() : str.length()));
      if (startIter) ++startIter;
      if (lengthIter) ++lengthIter;

      auto s2 = string_replace(str, nStart, nLength, repl);
      ret.append(s2);
    }
  }
  return ret;
}