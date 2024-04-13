TEST_P(LocalJwksIntegrationTest, ExpiredTokenHeadReply) {
  config_helper_.prependFilter(getFilterConfig(true));
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "HEAD"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"Authorization", "Bearer " + std::string(ExpiredToken)},
  });

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("401", response->headers().getStatusValue());
  EXPECT_EQ(
      "Bearer realm=\"http://host/\", error=\"invalid_token\"",
      response->headers().get(Http::Headers::get().WWWAuthenticate)[0]->value().getStringView());

  EXPECT_NE("0", response->headers().getContentLengthValue());
  EXPECT_THAT(response->body(), ::testing::IsEmpty());
}