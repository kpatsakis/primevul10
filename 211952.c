Variant HHVM_FUNCTION(HH_first_key, const Variant& v) {
  // 1. array, vec, dict, keyset
  if (v.isArray() || v.isClsMeth()) {
    auto arr = v.toArray();
    if (arr->empty()) {
      return init_null();
    }
    return arr->getKey(arr->iter_begin());
  }

  if (v.isObject()) {
    auto obj = v.asCObjRef();
    // 2. collection
    if (obj->isCollection()) {
      // Pair
      if (obj->collectionType() == CollectionType::Pair) {
        return Variant::wrap(make_tv<KindOfInt64>(0));
      }

      // Vector, Map, Set, and Imm variants
      auto arr = collections::asArray(obj.get());
      if (arr->empty()) {
        return init_null();
      }
      return arr->getKey(arr->iter_begin());
    }
  }
  SystemLib::throwInvalidArgumentExceptionObject(
    "Argument 1 passed to HH\\Lib\\_Private\\Native\\first_key() "
    "must be a Container");
}