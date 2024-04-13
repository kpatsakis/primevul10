  static req::ptr<CSRequest> Get(const Variant& var) {
    auto csr = cast_or_null<CSRequest>(GetRequest(var));
    if (!csr || !csr->m_csr) {
      raise_warning("cannot get CSR");
      return nullptr;
    }
    return csr;
  }