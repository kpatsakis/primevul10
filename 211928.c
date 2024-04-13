static req::ptr<MCrypt> get_valid_mcrypt_resource(const Resource& td) {
  auto pm = dyn_cast_or_null<MCrypt>(td);

  if (pm == nullptr || pm->isInvalid()) {
    raise_warning("supplied argument is not a valid MCrypt resource");
    return nullptr;
  }

  return pm;
}