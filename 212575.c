static void object_set(const json_parser* json,
                       Variant &var,
                       const String& key,
                       const Variant& value,
                       int assoc,
                       JSONContainerType container_type) {
  if (!assoc) {
    // We know it is stdClass, and everything is public (and dynamic).
    if (key.empty()) {
      var.getObjectData()->setProp(nullptr, s__empty_.get(), *value.asTypedValue());
    } else {
      var.getObjectData()->o_set(key, value);
    }
  } else {
    if (container_type == JSONContainerType::COLLECTIONS) {
      auto keyTV = make_tv<KindOfString>(key.get());
      collections::set(var.getObjectData(), &keyTV, value.asTypedValue());
    } else if (container_type == JSONContainerType::HACK_ARRAYS ||
               container_type == JSONContainerType::LEGACY_HACK_ARRAYS) {
      forceToDict(var).set(key, value);
    } else {
      int64_t i;
      if (key.get()->isStrictlyInteger(i)) {
        forceToDArray(var).set(i, value);
      } else {
        forceToDArray(var).set(key, value);
      }
    }
    if (var.isArray()) {
      DEBUG_ONLY auto const data = var.getArrayData();
      assertx(IMPLIES(arrprov::arrayWantsTag(data), arrprov::getTag(data)));
    }
  }
}