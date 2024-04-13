PseudoHandle<JSObject> JSObject::create(
    Runtime *runtime,
    Handle<HiddenClass> clazz) {
  auto obj = JSObject::create(runtime, clazz->getNumProperties());
  obj->clazz_.set(runtime, *clazz, &runtime->getHeap());
  // If the hidden class has index like property, we need to clear the fast path
  // flag.
  if (LLVM_UNLIKELY(obj->clazz_.get(runtime)->getHasIndexLikeProperties()))
    obj->flags_.fastIndexProperties = false;
  return obj;
}