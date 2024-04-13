  bool parsePacked() {
    auto const fp = top;
    if (!matchSeparator(']')) {
      if (++array_depth >= 0) return false;
      do {
        if (!parseValue(true)) return false;
      } while (matchSeparator(','));
      --array_depth;
      if (!matchSeparator(']')) return false;  // Trailing ',' not supported.
    }
    auto arr = [&] {
      if (container_type == JSONContainerType::HACK_ARRAYS) {
        return top == fp
          ? ArrayData::CreateVec()
          : PackedArray::MakeVecNatural(top - fp, fp);
      }
      if (container_type == JSONContainerType::LEGACY_HACK_ARRAYS) {
        auto ret = top == fp
          ? staticEmptyVecArray()->copy()
          : PackedArray::MakeVecNatural(top - fp, fp);
        ret->setLegacyArray(true);
        return ret;
      }
      if (container_type == JSONContainerType::DARRAYS_AND_VARRAYS) {
        return top == fp
          ? ArrayData::CreateVArray()
          : PackedArray::MakeVArrayNatural(top - fp, fp);
      }
      assertx(container_type == JSONContainerType::DARRAYS);
      return top == fp
        ? ArrayData::CreateDArray()
        : MixedArray::MakeDArrayNatural(top - fp, fp);
    }();
    top = fp;
    pushArrayData(arr);
    check_non_safepoint_surprise();
    return true;
  }