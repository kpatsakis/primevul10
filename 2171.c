  const absl::flat_hash_map<const NodeDef*, int> TopoOrder(
      const std::vector<const NodeDef*>& topo_order) const {
    absl::flat_hash_map<const NodeDef*, int> map;
    map.reserve(topo_order.size());
    for (int i = 0, topo_order_size = topo_order.size(); i < topo_order_size;
         ++i) {
      map.emplace(topo_order[i], i);
    }
    return map;
  }