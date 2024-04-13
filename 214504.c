TEST_P(LocalJwksIntegrationTest, MissingToken) {
  config_helper_.prependFilter(getFilterConfig(true));
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
  });

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("401", response->headers().getStatusValue());
  EXPECT_EQ(
      "Bearer realm=\"http://host/\"",
      response->headers().get(Http::Headers::get().WWWAuthenticate)[0]->value().getStringView());
}