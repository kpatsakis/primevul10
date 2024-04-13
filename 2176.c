    bool operator==(const ShapeId& other) const {
      return node == other.node && port_id == other.port_id;
    }