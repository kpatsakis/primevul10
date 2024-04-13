CallResult<uint32_t> appendAllPropertyNames(
    Handle<JSObject> obj,
    Runtime *runtime,
    MutableHandle<BigStorage> &arr,
    uint32_t beginIndex) {
  uint32_t size = beginIndex;
  // We know that duplicate property names can only exist between objects in
  // the prototype chain. Hence there should not be duplicated properties
  // before we start to look at any prototype.
  bool needDedup = false;
  MutableHandle<> prop(runtime);
  MutableHandle<JSObject> head(runtime, obj.get());
  MutableHandle<StringPrimitive> tmpVal{runtime};
  while (head.get()) {
    GCScope gcScope(runtime);

    // enumerableProps will contain all enumerable own properties from obj.
    // Impl note: this is the only place where getOwnPropertyKeys will be
    // called without IncludeNonEnumerable on a Proxy.  Everywhere else,
    // trap ordering is specified but ES9 13.7.5.15 says "The mechanics and
    // order of enumerating the properties is not specified", which is
    // unusual.
    auto cr =
        JSObject::getOwnPropertyNames(head, runtime, true /* onlyEnumerable */);
    if (LLVM_UNLIKELY(cr == ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }
    auto enumerableProps = *cr;
    auto marker = gcScope.createMarker();
    for (unsigned i = 0, e = enumerableProps->getEndIndex(); i < e; ++i) {
      gcScope.flushToMarker(marker);
      prop = enumerableProps->at(runtime, i);
      if (!needDedup) {
        // If no dedup is needed, add it directly.
        if (LLVM_UNLIKELY(
                BigStorage::push_back(arr, runtime, prop) ==
                ExecutionStatus::EXCEPTION)) {
          return ExecutionStatus::EXCEPTION;
        }
        ++size;
        continue;
      }
      // Otherwise loop through all existing properties and check if we
      // have seen it before.
      bool dupFound = false;
      if (prop->isNumber()) {
        for (uint32_t j = beginIndex; j < size && !dupFound; ++j) {
          HermesValue val = arr->at(j);
          if (val.isNumber()) {
            dupFound = val.getNumber() == prop->getNumber();
          } else {
            // val is string, prop is number.
            tmpVal = val.getString();
            auto valNum = toArrayIndex(
                StringPrimitive::createStringView(runtime, tmpVal));
            dupFound = valNum && valNum.getValue() == prop->getNumber();
          }
        }
      } else {
        for (uint32_t j = beginIndex; j < size && !dupFound; ++j) {
          HermesValue val = arr->at(j);
          if (val.isNumber()) {
            // val is number, prop is string.
            auto propNum = toArrayIndex(StringPrimitive::createStringView(
                runtime, Handle<StringPrimitive>::vmcast(prop)));
            dupFound = propNum && (propNum.getValue() == val.getNumber());
          } else {
            dupFound = val.getString()->equals(prop->getString());
          }
        }
      }
      if (LLVM_LIKELY(!dupFound)) {
        if (LLVM_UNLIKELY(
                BigStorage::push_back(arr, runtime, prop) ==
                ExecutionStatus::EXCEPTION)) {
          return ExecutionStatus::EXCEPTION;
        }
        ++size;
      }
    }
    // Continue to follow the prototype chain.
    CallResult<PseudoHandle<JSObject>> parentRes =
        JSObject::getPrototypeOf(head, runtime);
    if (LLVM_UNLIKELY(parentRes == ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }
    head = parentRes->get();
    needDedup = true;
  }
  return size;
}