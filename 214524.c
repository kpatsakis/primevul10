  void waitForJwksResponse(const std::string& status, const std::string& jwks_body) {
    AssertionResult result =
        fake_upstreams_[1]->waitForHttpConnection(*dispatcher_, fake_jwks_connection_);
    RELEASE_ASSERT(result, result.message());
    result = fake_jwks_connection_->waitForNewStream(*dispatcher_, jwks_request_);
    RELEASE_ASSERT(result, result.message());
    result = jwks_request_->waitForEndStream(*dispatcher_);
    RELEASE_ASSERT(result, result.message());

    Http::TestResponseHeaderMapImpl response_headers{{":status", status}};
    jwks_request_->encodeHeaders(response_headers, false);
    Buffer::OwnedImpl response_data1(jwks_body);
    jwks_request_->encodeData(response_data1, true);
  }