  void ResolveStat(const uv_stat_t* stat) override {
    Resolve(node::FillStatsArray(&stats_field_array_, stat));
  }