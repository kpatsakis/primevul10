  void createUpstreams() override {
    HttpProtocolIntegrationTest::createUpstreams();
    // for Jwks upstream.
    addFakeUpstream(GetParam().upstream_protocol);
  }