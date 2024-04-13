JSObject::checkPropertyUpdate(
    Runtime *runtime,
    const PropertyFlags currentFlags,
    DefinePropertyFlags dpFlags,
    const HermesValue curValueOrAccessor,
    Handle<> valueOrAccessor,
    PropOpFlags opFlags) {
  // 8.12.9 [5] Return true, if every field in Desc is absent.
  if (dpFlags.isEmpty())
    return std::make_pair(PropertyUpdateStatus::done, currentFlags);

  assert(
      (!dpFlags.isAccessor() || (!dpFlags.setWritable && !dpFlags.writable)) &&
      "can't set both accessor and writable");
  assert(
      !dpFlags.enableInternalSetter &&
      "cannot change the value of internalSetter");

  // 8.12.9 [6] Return true, if every field in Desc also occurs in current and
  // the value of every field in Desc is the same value as the corresponding
  // field in current when compared using the SameValue algorithm (9.12).
  // TODO: this would probably be much more efficient with bitmasks.
  if ((!dpFlags.setEnumerable ||
       dpFlags.enumerable == currentFlags.enumerable) &&
      (!dpFlags.setConfigurable ||
       dpFlags.configurable == currentFlags.configurable)) {
    if (dpFlags.isAccessor()) {
      if (currentFlags.accessor) {
        auto *curAccessor = vmcast<PropertyAccessor>(curValueOrAccessor);
        auto *newAccessor = vmcast<PropertyAccessor>(valueOrAccessor.get());

        if ((!dpFlags.setGetter ||
             curAccessor->getter == newAccessor->getter) &&
            (!dpFlags.setSetter ||
             curAccessor->setter == newAccessor->setter)) {
          return std::make_pair(PropertyUpdateStatus::done, currentFlags);
        }
      }
    } else {
      if (!currentFlags.accessor &&
          (!dpFlags.setValue ||
           isSameValue(curValueOrAccessor, valueOrAccessor.get())) &&
          (!dpFlags.setWritable || dpFlags.writable == currentFlags.writable)) {
        return std::make_pair(PropertyUpdateStatus::done, currentFlags);
      }
    }
  }

  // 8.12.9 [7]
  // If the property is not configurable, some aspects are not changeable.
  if (!currentFlags.configurable) {
    // Trying to change non-configurable to configurable?
    if (dpFlags.configurable) {
      if (opFlags.getThrowOnError()) {
        return runtime->raiseTypeError(
            "property is not configurable"); // TODO: better message.
      }
      return std::make_pair(PropertyUpdateStatus::failed, PropertyFlags{});
    }

    // Trying to change the enumerability of non-configurable property?
    if (dpFlags.setEnumerable &&
        dpFlags.enumerable != currentFlags.enumerable) {
      if (opFlags.getThrowOnError()) {
        return runtime->raiseTypeError(
            "property is not configurable"); // TODO: better message.
      }
      return std::make_pair(PropertyUpdateStatus::failed, PropertyFlags{});
    }
  }

  PropertyFlags newFlags = currentFlags;

  // 8.12.9 [8] If IsGenericDescriptor(Desc) is true, then no further validation
  // is required.
  if (!(dpFlags.setValue || dpFlags.setWritable || dpFlags.setGetter ||
        dpFlags.setSetter)) {
    // Do nothing
  }
  // 8.12.9 [9]
  // Changing between accessor and data descriptor?
  else if (currentFlags.accessor != dpFlags.isAccessor()) {
    if (!currentFlags.configurable) {
      if (opFlags.getThrowOnError()) {
        return runtime->raiseTypeError(
            "property is not configurable"); // TODO: better message.
      }
      return std::make_pair(PropertyUpdateStatus::failed, PropertyFlags{});
    }

    // If we change from accessor to data descriptor, Preserve the existing
    // values of the converted property’s [[Configurable]] and [[Enumerable]]
    // attributes and set the rest of the property’s attributes to their default
    // values.
    // If it's the other way around, since the accessor doesn't have the
    // [[Writable]] attribute, do nothing.
    newFlags.writable = 0;

    // If we are changing from accessor to non-accessor, we must set a new
    // value.
    if (!dpFlags.isAccessor())
      dpFlags.setValue = 1;
  }
  // 8.12.9 [10] if both are data descriptors.
  else if (!currentFlags.accessor) {
    if (!currentFlags.configurable) {
      if (!currentFlags.writable) {
        // If the current property is not writable, but the new one is.
        if (dpFlags.writable) {
          if (opFlags.getThrowOnError()) {
            return runtime->raiseTypeError(
                "property is not configurable"); // TODO: better message.
          }
          return std::make_pair(PropertyUpdateStatus::failed, PropertyFlags{});
        }

        // If we are setting a different value.
        if (dpFlags.setValue &&
            !isSameValue(curValueOrAccessor, valueOrAccessor.get())) {
          if (opFlags.getThrowOnError()) {
            return runtime->raiseTypeError(
                "property is not writable"); // TODO: better message.
          }
          return std::make_pair(PropertyUpdateStatus::failed, PropertyFlags{});
        }
      }
    }
  }
  // 8.12.9 [11] Both are accessors.
  else {
    auto *curAccessor = vmcast<PropertyAccessor>(curValueOrAccessor);
    auto *newAccessor = vmcast<PropertyAccessor>(valueOrAccessor.get());

    // If not configurable, make sure that nothing is changing.
    if (!currentFlags.configurable) {
      if ((dpFlags.setGetter && newAccessor->getter != curAccessor->getter) ||
          (dpFlags.setSetter && newAccessor->setter != curAccessor->setter)) {
        if (opFlags.getThrowOnError()) {
          return runtime->raiseTypeError(
              "property is not configurable"); // TODO: better message.
        }
        return std::make_pair(PropertyUpdateStatus::failed, PropertyFlags{});
      }
    }

    // If not setting the getter or the setter, re-use the current one.
    if (!dpFlags.setGetter)
      newAccessor->getter.set(
          runtime, curAccessor->getter, &runtime->getHeap());
    if (!dpFlags.setSetter)
      newAccessor->setter.set(
          runtime, curAccessor->setter, &runtime->getHeap());
  }

  // 8.12.9 [12] For each attribute field of Desc that is present, set the
  // correspondingly named attribute of the property named P of object O to the
  // value of the field.
  if (dpFlags.setEnumerable)
    newFlags.enumerable = dpFlags.enumerable;
  if (dpFlags.setWritable)
    newFlags.writable = dpFlags.writable;
  if (dpFlags.setConfigurable)
    newFlags.configurable = dpFlags.configurable;

  if (dpFlags.setValue)
    newFlags.accessor = false;
  else if (dpFlags.isAccessor())
    newFlags.accessor = true;
  else
    return std::make_pair(PropertyUpdateStatus::done, newFlags);

  return std::make_pair(PropertyUpdateStatus::needSet, newFlags);
}