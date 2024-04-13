Zone::Zone(Isolate* isolate)
    : zone_excess_limit_(256 * MB),
      segment_bytes_allocated_(0),
      position_(0),
      limit_(0),
      scope_nesting_(0),
      segment_head_(NULL),
      isolate_(isolate) {
}