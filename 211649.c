  static req::ptr<Key> Get(const Variant& var, bool public_key,
                           const char *passphrase = nullptr) {
    if (var.isArray()) {
      Array arr = var.toArray();
      if (!arr.exists(int64_t(0)) || !arr.exists(int64_t(1))) {
        raise_warning("key array must be of the form "
                        "array(0 => key, 1 => phrase)");
        return nullptr;
      }

      String zphrase = arr[1].toString();
      return GetHelper(arr[0], public_key, zphrase.data());
    }
    return GetHelper(var, public_key, passphrase);
  }