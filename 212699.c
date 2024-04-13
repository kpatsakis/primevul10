JSObject *JSObject::getNamedDescriptor(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    PropertyFlags expectedFlags,
    NamedPropertyDescriptor &desc) {
  if (findProperty(selfHandle, runtime, name, expectedFlags, desc))
    return *selfHandle;

  // Check here for host object flag.  This means that "normal" own
  // properties above win over host-defined properties, but there's no
  // cost imposed on own property lookups.  This should do what we
  // need in practice, and we can define host vs js property
  // disambiguation however we want.  This is here in order to avoid
  // impacting perf for the common case where an own property exists
  // in normal storage.
  if (LLVM_UNLIKELY(selfHandle->flags_.hostObject)) {
    desc.flags.hostObject = true;
    desc.flags.writable = true;
    return *selfHandle;
  }

  if (LLVM_UNLIKELY(selfHandle->flags_.lazyObject)) {
    assert(
        !selfHandle->flags_.proxyObject &&
        "Proxy objects should never be lazy");
    // Initialize the object and perform the lookup again.
    JSObject::initializeLazyObject(runtime, selfHandle);

    if (findProperty(selfHandle, runtime, name, expectedFlags, desc))
      return *selfHandle;
  }

  if (LLVM_UNLIKELY(selfHandle->flags_.proxyObject)) {
    desc.flags.proxyObject = true;
    return *selfHandle;
  }

  if (selfHandle->parent_) {
    MutableHandle<JSObject> mutableSelfHandle{
        runtime, selfHandle->parent_.getNonNull(runtime)};

    do {
      // Check the most common case first, at the cost of some code duplication.
      if (LLVM_LIKELY(
              !mutableSelfHandle->flags_.lazyObject &&
              !mutableSelfHandle->flags_.hostObject &&
              !mutableSelfHandle->flags_.proxyObject)) {
      findProp:
        if (findProperty(
                mutableSelfHandle,
                runtime,
                name,
                PropertyFlags::invalid(),
                desc)) {
          assert(
              !selfHandle->flags_.proxyObject &&
              "Proxy object parents should never have own properties");
          return *mutableSelfHandle;
        }
      } else if (LLVM_UNLIKELY(mutableSelfHandle->flags_.lazyObject)) {
        JSObject::initializeLazyObject(runtime, mutableSelfHandle);
        goto findProp;
      } else if (LLVM_UNLIKELY(mutableSelfHandle->flags_.hostObject)) {
        desc.flags.hostObject = true;
        desc.flags.writable = true;
        return *mutableSelfHandle;
      } else {
        assert(
            mutableSelfHandle->flags_.proxyObject &&
            "descriptor flags are impossible");
        desc.flags.proxyObject = true;
        return *mutableSelfHandle;
      }
    } while ((mutableSelfHandle = mutableSelfHandle->parent_.get(runtime)));
  }

  return nullptr;
}