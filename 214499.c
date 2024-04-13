  void initializeAsyncFetchFilter(bool fast_listener) {
    config_helper_.prependFilter(getAsyncFetchFilterConfig(ExampleConfig, fast_listener));

    config_helper_.addConfigModifier([](envoy::config::bootstrap::v3::Bootstrap& bootstrap) {
      auto* jwks_cluster = bootstrap.mutable_static_resources()->add_clusters();
      jwks_cluster->MergeFrom(bootstrap.static_resources().clusters()[0]);
      jwks_cluster->set_name("pubkey_cluster");
    });

    initialize();
  }