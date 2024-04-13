uint32_t matchesProtoClasses(
    Runtime *runtime,
    Handle<JSObject> obj,
    Handle<BigStorage> arr) {
  MutableHandle<JSObject> head(runtime, obj->getParent(runtime));
  uint32_t i = 0;
  while (head.get()) {
    HermesValue protoCls = arr->at(i++);
    if (protoCls.isNull() || protoCls.getObject() != head->getClass(runtime) ||
        head->isProxyObject()) {
      return 0;
    }
    head = head->getParent(runtime);
  }
  // The chains must both end at the same point.
  if (head || !arr->at(i++).isNull()) {
    return 0;
  }
  assert(i > 0 && "success should be positive");
  return i;
}