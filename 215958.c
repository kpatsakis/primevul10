bool NodeNameInValues(const std::vector<string>& control_dependencies,
                      const StringPiece& node_name) {
  return std::find(control_dependencies.begin(), control_dependencies.end(),
                   node_name) != control_dependencies.end();
}