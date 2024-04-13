JSONContainerType get_container_type_from_options(int64_t options) {
  if ((options & k_JSON_FB_STABLE_MAPS) ||
      (options & k_JSON_FB_COLLECTIONS)) {
    return JSONContainerType::COLLECTIONS;
  }

  if (options & k_JSON_FB_HACK_ARRAYS) {
    return JSONContainerType::HACK_ARRAYS;
  }

  if (options & k_JSON_FB_DARRAYS) {
    return JSONContainerType::DARRAYS;
  }

  if (options & k_JSON_FB_DARRAYS_AND_VARRAYS) {
    return JSONContainerType::DARRAYS_AND_VARRAYS;
  }

  if (options & k_JSON_FB_LEGACY_HACK_ARRAYS) {
    return JSONContainerType::LEGACY_HACK_ARRAYS;
  }

  return JSONContainerType::DARRAYS;
}