  explicit TopoQueue(const std::vector<const NodeDef*>& topo_order)
      : topo_order_(TopoOrder(topo_order)) {}