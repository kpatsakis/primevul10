    std::size_t operator()(const DimId& dim) const {
      return std::hash<const NodeDef*>{}(dim.node) + dim.port_id +
             dim.dim_index;
    }