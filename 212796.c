CallResult<Handle<BigStorage>> getForInPropertyNames(
    Runtime *runtime,
    Handle<JSObject> obj,
    uint32_t &beginIndex,
    uint32_t &endIndex) {
  Handle<HiddenClass> clazz(runtime, obj->getClass(runtime));

  // Fast case: Check the cache.
  MutableHandle<BigStorage> arr(runtime, clazz->getForInCache(runtime));
  if (arr) {
    beginIndex = matchesProtoClasses(runtime, obj, arr);
    if (beginIndex) {
      // Cache is valid for this object, so use it.
      endIndex = arr->size();
      return arr;
    }
    // Invalid for this object. We choose to clear the cache since the
    // changes to the prototype chain probably affect other objects too.
    clazz->clearForInCache(runtime);
    // Clear arr to slightly reduce risk of OOM from allocation below.
    arr = nullptr;
  }

  // Slow case: Build the array of properties.
  auto ownPropEstimate = clazz->getNumProperties();
  auto arrRes = obj->shouldCacheForIn(runtime)
      ? BigStorage::createLongLived(runtime, ownPropEstimate)
      : BigStorage::create(runtime, ownPropEstimate);
  if (LLVM_UNLIKELY(arrRes == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  arr = std::move(*arrRes);
  if (setProtoClasses(runtime, obj, arr) == ExecutionStatus::EXCEPTION) {
    return ExecutionStatus::EXCEPTION;
  }
  beginIndex = arr->size();
  // If obj or any of its prototypes are unsuitable for caching, then
  // beginIndex is 0 and we return an array with only the property names.
  bool canCache = beginIndex;
  auto end = appendAllPropertyNames(obj, runtime, arr, beginIndex);
  if (end == ExecutionStatus::EXCEPTION) {
    return ExecutionStatus::EXCEPTION;
  }
  endIndex = *end;
  // Avoid degenerate memory explosion: if > 75% of the array is properties
  // or classes from prototypes, then don't cache it.
  const bool tooMuchProto = *end / 4 > ownPropEstimate;
  if (canCache && !tooMuchProto) {
    assert(beginIndex > 0 && "cached array must start with proto classes");
#ifdef HERMES_SLOW_DEBUG
    assert(beginIndex == matchesProtoClasses(runtime, obj, arr) && "matches");
#endif
    clazz->setForInCache(*arr, runtime);
  }
  return arr;
}