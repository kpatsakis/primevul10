void JSObject::allocateNewSlotStorage(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SlotIndex newSlotIndex,
    Handle<> valueHandle) {
  // If it is a direct property, just store the value and we are done.
  if (LLVM_LIKELY(newSlotIndex < DIRECT_PROPERTY_SLOTS)) {
    selfHandle->directProps()[newSlotIndex].set(
        *valueHandle, &runtime->getHeap());
    return;
  }

  // Make the slot index relative to the indirect storage.
  newSlotIndex -= DIRECT_PROPERTY_SLOTS;

  // Allocate a new property storage if not already allocated.
  if (LLVM_UNLIKELY(!selfHandle->propStorage_)) {
    // Allocate new storage.
    assert(newSlotIndex == 0 && "allocated slot must be at end");
    auto arrRes = runtime->ignoreAllocationFailure(
        PropStorage::create(runtime, DEFAULT_PROPERTY_CAPACITY));
    selfHandle->propStorage_.set(
        runtime, vmcast<PropStorage>(arrRes), &runtime->getHeap());
  } else if (LLVM_UNLIKELY(
                 newSlotIndex >=
                 selfHandle->propStorage_.get(runtime)->capacity())) {
    // Reallocate the existing one.
    assert(
        newSlotIndex == selfHandle->propStorage_.get(runtime)->size() &&
        "allocated slot must be at end");
    auto hnd = runtime->makeMutableHandle(selfHandle->propStorage_);
    PropStorage::resize(hnd, runtime, newSlotIndex + 1);
    selfHandle->propStorage_.set(runtime, *hnd, &runtime->getHeap());
  }

  {
    NoAllocScope scope{runtime};
    auto *const propStorage = selfHandle->propStorage_.getNonNull(runtime);
    if (newSlotIndex >= propStorage->size()) {
      assert(
          newSlotIndex == propStorage->size() &&
          "allocated slot must be at end");
      PropStorage::resizeWithinCapacity(propStorage, runtime, newSlotIndex + 1);
    }
    // If we don't need to resize, just store it directly.
    propStorage->at(newSlotIndex).set(*valueHandle, &runtime->getHeap());
  }
}