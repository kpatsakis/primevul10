void JSObject::updatePropertyFlagsWithoutTransitions(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    PropertyFlags flagsToClear,
    PropertyFlags flagsToSet,
    OptValue<llvh::ArrayRef<SymbolID>> props) {
  auto newClazz = HiddenClass::updatePropertyFlagsWithoutTransitions(
      runtime->makeHandle(selfHandle->clazz_),
      runtime,
      flagsToClear,
      flagsToSet,
      props);
  selfHandle->clazz_.set(runtime, *newClazz, &runtime->getHeap());
}