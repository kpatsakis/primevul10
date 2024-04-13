  void setup(const std::string& filter_config, const PerRouteConfig& per_route) {
    config_helper_.prependFilter(getAuthFilterConfig(filter_config, true));

    config_helper_.addConfigModifier(
        [per_route](
            envoy::extensions::filters::network::http_connection_manager::v3::HttpConnectionManager&
                hcm) {
          auto* virtual_host = hcm.mutable_route_config()->mutable_virtual_hosts(0);
          auto& per_route_any =
              (*virtual_host->mutable_routes(0)
                    ->mutable_typed_per_filter_config())["envoy.filters.http.jwt_authn"];
          per_route_any.PackFrom(per_route);
        });

    initialize();
  }