void JSObject::initializeLazyObject(
    Runtime *runtime,
    Handle<JSObject> lazyObject) {
  assert(lazyObject->flags_.lazyObject && "object must be lazy");
  // object is now assumed to be a regular object.
  lazyObject->flags_.lazyObject = 0;

  // only functions can be lazy.
  assert(vmisa<Callable>(lazyObject.get()) && "unexpected lazy object");
  Callable::defineLazyProperties(Handle<Callable>::vmcast(lazyObject), runtime);
}