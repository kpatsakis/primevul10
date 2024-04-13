Variant HHVM_FUNCTION(HH_last, const Variant& v) {
  // 1. array, vec, dict, keyset
  if (v.isArray() || v.isClsMeth()) {
    auto arr = v.toArray();
    if (arr->empty()) {
      return init_null();
    }
    return arr->getValue(arr->iter_last());
  }

  if (v.isObject()) {
    auto obj = v.asCObjRef();
    // 2. collection
    if (obj->isCollection()) {
      // Pair
      if (obj->collectionType() == CollectionType::Pair) {
        auto const pair = static_cast<c_Pair*>(obj.get());
        return Variant::wrap(*pair->at(1));
      }

      // Vector, Map, Set, and Imm variants
      auto arr = collections::asArray(obj.get());
      if (arr->empty()) {
        return init_null();
      }
      return arr->getValue(arr->iter_last());
    }
  }
  SystemLib::throwInvalidArgumentExceptionObject(
    "Argument 1 passed to HH\\Lib\\_Private\\Native\\last() "
    "must be a Container");
}