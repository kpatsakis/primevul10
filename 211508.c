  String rval(const StringData *key) const {
    not_reached();
    return rvalImpl(key ? key->toInt32() : 0);
  }