bool JSObject::isFrozen(PseudoHandle<JSObject> self, Runtime *runtime) {
  if (self->flags_.frozen)
    return true;
  if (!self->flags_.noExtend)
    return false;

  auto selfHandle = runtime->makeHandle(std::move(self));

  if (!HiddenClass::areAllReadOnly(
          runtime->makeHandle(selfHandle->clazz_), runtime)) {
    return false;
  }

  if (!checkAllOwnIndexed(
          *selfHandle,
          runtime,
          ObjectVTable::CheckAllOwnIndexedMode::ReadOnly)) {
    return false;
  }

  // Now that we know we are sealed, set the flag.
  selfHandle->flags_.frozen = true;
  selfHandle->flags_.sealed = true;
  return true;
}