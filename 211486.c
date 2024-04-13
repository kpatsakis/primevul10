Variant HHVM_FUNCTION(unserialize, const String& str,
                                   const Array& options) {
  return unserialize_from_string(
    str,
    VariableUnserializer::Type::Serialize,
    options
  );
}