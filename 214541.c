std::string getAuthFilterConfig(const std::string& config_str, bool use_local_jwks) {
  JwtAuthentication proto_config;
  TestUtility::loadFromYaml(config_str, proto_config);

  if (use_local_jwks) {
    auto& provider0 = (*proto_config.mutable_providers())[std::string(ProviderName)];
    provider0.clear_remote_jwks();
    auto local_jwks = provider0.mutable_local_jwks();
    local_jwks->set_inline_string(PublicKey);
  }

  HttpFilter filter;
  filter.set_name("envoy.filters.http.jwt_authn");
  filter.mutable_typed_config()->PackFrom(proto_config);
  return MessageUtil::getJsonStringFromMessageOrDie(filter);
}