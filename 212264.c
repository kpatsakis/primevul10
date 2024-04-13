  ~CSRequest() override {
    // X509_REQ_free(nullptr) is a no-op
    X509_REQ_free(m_csr);
  }