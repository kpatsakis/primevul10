  void initializeFilter(bool add_cluster) {
    config_helper_.prependFilter(getFilterConfig(false));

    if (add_cluster) {
      config_helper_.addConfigModifier([](envoy::config::bootstrap::v3::Bootstrap& bootstrap) {
        auto* jwks_cluster = bootstrap.mutable_static_resources()->add_clusters();
        jwks_cluster->MergeFrom(bootstrap.static_resources().clusters()[0]);
        jwks_cluster->set_name("pubkey_cluster");
      });
    } else {
      config_helper_.skipPortUsageValidation();
    }

    initialize();
  }