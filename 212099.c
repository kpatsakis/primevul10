  void format(FormatArg& arg, Callback& cb) const {
    FormatValue<HPHP::StringData*>(m_val.get()).format(arg, cb);
  }