  bool parseMixed() {
    auto const fp = top;
    if (!matchSeparator('}')) {
      if (++array_depth >= 0) return false;
      do {
        if (!matchSeparator('\"')) return false;  // Only support string keys.
        if (!parseMixedKey()) return false;
        // TODO(14491721): Precompute and save hash to avoid deref in MakeMixed.
        if (!matchSeparator(':')) return false;
        if (!parseValue(true)) return false;
      } while (matchSeparator(','));
      --array_depth;
      if (!matchSeparator('}')) return false;  // Trailing ',' not supported.
    }
    auto arr = [&] {
      if (container_type == JSONContainerType::HACK_ARRAYS) {
        return top == fp
          ? ArrayData::CreateDict()
          : MixedArray::MakeDict((top - fp) >> 1, fp)->asArrayData();
      }
      if (container_type == JSONContainerType::LEGACY_HACK_ARRAYS) {
        auto ret = top == fp
          ? staticEmptyDictArray()->copy()
          : MixedArray::MakeDict((top - fp) >> 1, fp)->asArrayData();
        ret->setLegacyArray(true);
        return ret;
      }
      assertx(container_type == JSONContainerType::DARRAYS ||
              container_type == JSONContainerType::DARRAYS_AND_VARRAYS);
      return top == fp
        ? ArrayData::CreateDArray()
        : MixedArray::MakeDArray((top - fp) >> 1, fp)->asArrayData();
    }();
    // MixedArray::MakeMixed can return nullptr if there are duplicate keys
    if (!arr) return false;
    top = fp;
    pushArrayData(arr);
    check_non_safepoint_surprise();
    return true;
  }