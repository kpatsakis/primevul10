bool Zone::excess_allocation() {
  return segment_bytes_allocated_ > zone_excess_limit_;
}