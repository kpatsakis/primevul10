  static String attach(StringData* sd) {
    return String(sd, NoIncRef{});
  }