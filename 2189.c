bool GraphProperties::HasOutputProperties(const string& node_name) const {
  return output_properties_.find(node_name) != output_properties_.end();
}