Status MaybeAppendVersionWarning(const VersionDef* versions,
                                 const Status& import_status) {
  if (versions && ForwardCompatibilityWindowPassed(*versions)) {
    return Status(
        import_status.code(),
        absl::StrCat(
            "Converting GraphDef to Graph has failed with an error: '",
            import_status.error_message(),
            "' The binary trying to import the GraphDef was built when "
            "GraphDef version was ",
            TF_GRAPH_DEF_VERSION,
            ". The GraphDef was produced by a binary built when GraphDef "
            "version was ",
            versions->producer(),
            ". The difference between these versions is larger than "
            "TensorFlow's forward compatibility guarantee, and might be the "
            "root cause for failing to import the GraphDef."));
  }
  return import_status;
}