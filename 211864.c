  ~Key() override {
    if (m_key) EVP_PKEY_free(m_key);
  }