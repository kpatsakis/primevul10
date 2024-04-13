  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     printing::MarginType* out) {
    std::string type;
    if (ConvertFromV8(isolate, val, &type)) {
      if (type == "default") {
        *out = printing::DEFAULT_MARGINS;
        return true;
      }
      if (type == "none") {
        *out = printing::NO_MARGINS;
        return true;
      }
      if (type == "printableArea") {
        *out = printing::PRINTABLE_AREA_MARGINS;
        return true;
      }
      if (type == "custom") {
        *out = printing::CUSTOM_MARGINS;
        return true;
      }
    }
    return false;
  }