Status GraphConstructor::UpdateVersionDef() {
  if (versions() == nullptr) return Status::OK();

  if (!opts_.importing) {
    g_->set_versions(*versions());
    return Status::OK();
  }
  VersionDef g_versions = g_->versions();
  g_versions.set_producer(
      std::min(g_versions.producer(), versions()->producer()));
  g_versions.set_min_consumer(
      std::max(g_versions.min_consumer(), versions()->min_consumer()));
  if (versions()->bad_consumers_size() > 0) {
    std::set<int> bad(g_versions.bad_consumers().begin(),
                      g_versions.bad_consumers().end());
    bad.insert(versions()->bad_consumers().begin(),
               versions()->bad_consumers().end());
    g_versions.clear_bad_consumers();
    for (int v : bad) {
      g_versions.add_bad_consumers(v);
    }
  }
  g_->set_versions(g_versions);
  return Status::OK();
}