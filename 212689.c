ObjectID JSObject::getObjectID(JSObject *self, Runtime *runtime) {
  if (LLVM_LIKELY(self->flags_.objectID))
    return self->flags_.objectID;

  // Object ID does not yet exist, get next unique global ID..
  self->flags_.objectID = runtime->generateNextObjectID();
  // Make sure it is not zero.
  if (LLVM_UNLIKELY(!self->flags_.objectID))
    --self->flags_.objectID;
  return self->flags_.objectID;
}