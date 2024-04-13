bool ForwardCompatibilityWindowPassed(const VersionDef& versions) {
  // TF_GRAPH_DEF_VERSION is incremented daily.
  // TF has a 3 week forward compatibility guarantee.
  return (versions.producer() - TF_GRAPH_DEF_VERSION) > 21;
}