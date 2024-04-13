  void cleanup() {
    codec_client_->close();
    if (fake_jwks_connection_ != nullptr) {
      AssertionResult result = fake_jwks_connection_->close();
      RELEASE_ASSERT(result, result.message());
      result = fake_jwks_connection_->waitForDisconnect();
      RELEASE_ASSERT(result, result.message());
    }
    if (fake_upstream_connection_ != nullptr) {
      AssertionResult result = fake_upstream_connection_->close();
      RELEASE_ASSERT(result, result.message());
      result = fake_upstream_connection_->waitForDisconnect();
      RELEASE_ASSERT(result, result.message());
    }
  }