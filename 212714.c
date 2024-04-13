bool JSObject::isSealed(PseudoHandle<JSObject> self, Runtime *runtime) {
  if (self->flags_.sealed)
    return true;
  if (!self->flags_.noExtend)
    return false;

  auto selfHandle = runtime->makeHandle(std::move(self));

  if (!HiddenClass::areAllNonConfigurable(
          runtime->makeHandle(selfHandle->clazz_), runtime)) {
    return false;
  }

  if (!checkAllOwnIndexed(
          *selfHandle,
          runtime,
          ObjectVTable::CheckAllOwnIndexedMode::NonConfigurable)) {
    return false;
  }

  // Now that we know we are sealed, set the flag.
  selfHandle->flags_.sealed = true;
  return true;
}