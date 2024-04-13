ZoneScope::ZoneScope(Zone* zone, ZoneScopeMode mode)
    : zone_(zone), mode_(mode) {
  zone_->scope_nesting_++;
}