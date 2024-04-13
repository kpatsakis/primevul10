void Zone::adjust_segment_bytes_allocated(int delta) {
  segment_bytes_allocated_ += delta;
  isolate_->counters()->zone_segment_bytes()->Set(segment_bytes_allocated_);
}