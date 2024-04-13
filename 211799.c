String HHVM_FUNCTION(HH_serialize_with_options,
                     const Variant& value, const Array& options) {
  SerializeOptions opts;
  opts.keepDVArrays = options.exists(s_keepDVArrays) &&
    options[s_keepDVArrays].toBoolean();
  opts.forcePHPArrays = options.exists(s_forcePHPArrays) &&
    options[s_forcePHPArrays].toBoolean();
  opts.warnOnHackArrays = options.exists(s_warnOnHackArrays) &&
    options[s_warnOnHackArrays].toBoolean();
  opts.warnOnPHPArrays = options.exists(s_warnOnPHPArrays) &&
    options[s_warnOnPHPArrays].toBoolean();
  opts.ignoreLateInit = options.exists(s_ignoreLateInit) &&
    options[s_ignoreLateInit].toBoolean();
  opts.serializeProvenanceAndLegacy =
    options.exists(s_serializeProvenanceAndLegacy) &&
    options[s_serializeProvenanceAndLegacy].toBoolean();
  return serialize_impl(value, opts);
}