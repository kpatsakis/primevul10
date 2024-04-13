  static req::ptr<CSRequest> GetRequest(const Variant& var) {
    if (var.isResource()) {
      return dyn_cast_or_null<CSRequest>(var);
    }
    if (var.isString() || var.isObject()) {
      BIO *in = Certificate::ReadData(var);
      if (in == nullptr) return nullptr;

      X509_REQ *csr = PEM_read_bio_X509_REQ(in, nullptr,nullptr,nullptr);
      BIO_free(in);
      if (csr) {
        return req::make<CSRequest>(csr);
      }
    }
    return nullptr;
  }