  String rval(const char* key) const {
    return rvalImpl(String(key).toInt32());
  }