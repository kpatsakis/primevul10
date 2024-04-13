bool HHVM_FUNCTION(HH_is_late_init_prop_init,
                   const Object& obj,
                   const String& name) {
  auto const ctx = fromCaller(
    [] (const ActRec* fp, Offset) { return fp->func()->cls(); }
  );
  auto const val = obj->getPropIgnoreLateInit(ctx, name.get());
  if (!val) {
    SystemLib::throwInvalidArgumentExceptionObject(
      folly::sformat(
       "Unknown or inaccessible property '{}' on object of class {}",
       name.get(),
       obj->getVMClass()->name()
      )
    );
  }
  return type(val) != KindOfUninit;
}