std::string getAsyncFetchFilterConfig(const std::string& config_str, bool fast_listener) {
  JwtAuthentication proto_config;
  TestUtility::loadFromYaml(config_str, proto_config);

  auto& provider0 = (*proto_config.mutable_providers())[std::string(ProviderName)];
  auto* async_fetch = provider0.mutable_remote_jwks()->mutable_async_fetch();
  async_fetch->set_fast_listener(fast_listener);

  HttpFilter filter;
  filter.set_name("envoy.filters.http.jwt_authn");
  filter.mutable_typed_config()->PackFrom(proto_config);
  return MessageUtil::getJsonStringFromMessageOrDie(filter);
}