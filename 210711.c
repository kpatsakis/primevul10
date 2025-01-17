ContentSettingsStore::FindEntry(const std::string& ext_id) const {
  ExtensionEntryMap::const_iterator i;
  for (i = entries_.begin(); i != entries_.end(); ++i) {
    if (i->second->id == ext_id)
      return i;
  }
  return entries_.end();
}
