  bool operator<(const AttrKeyAndValue& b) const {
    if (key_name_ != b.key_name_) {
      return key_name_ < b.key_name_;
    } else if (key_suffix_ != b.key_suffix_) {
      return key_suffix_ < b.key_suffix_;
    } else {
      return value_ < b.value_;
    }
  }