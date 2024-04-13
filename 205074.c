inline void* ZoneAllocationPolicy::New(size_t size) {
  ASSERT(zone_);
  return zone_->New(static_cast<int>(size));
}