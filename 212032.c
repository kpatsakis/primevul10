bool HHVM_FUNCTION(HH_is_late_init_sprop_init,
                   const String& clsName,
                   const String& name) {
  auto const cls = Class::load(clsName.get());
  if (!cls) {
    SystemLib::throwInvalidArgumentExceptionObject(
      folly::sformat("Unknown class {}", clsName)
    );
  }
  auto const ctx = fromCaller(
    [] (const ActRec* fp, Offset) { return fp->func()->cls(); }
  );
  auto const lookup = cls->getSPropIgnoreLateInit(ctx, name.get());
  if (!lookup.val || !lookup.accessible) {
    SystemLib::throwInvalidArgumentExceptionObject(
      folly::sformat(
       "Unknown or inaccessible static property '{}' on class {}",
       name.get(),
       clsName.get()
      )
    );
  }
  return type(lookup.val) != KindOfUninit;
}