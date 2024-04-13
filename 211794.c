  explicit CSRequest(X509_REQ *csr) : m_csr(csr) {
    assertx(m_csr);
  }