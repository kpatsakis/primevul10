std::string getFilterConfig(bool use_local_jwks) {
  return getAuthFilterConfig(ExampleConfig, use_local_jwks);
}