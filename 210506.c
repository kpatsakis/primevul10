void ContentSettingsStore::RegisterExtension(
    const std::string& ext_id,
    const base::Time& install_time,
    bool is_enabled) {
  base::AutoLock lock(lock_);
  ExtensionEntryMap::iterator i = FindEntry(ext_id);
  if (i != entries_.end()) {
    delete i->second;
    entries_.erase(i);
  }

  ExtensionEntry* entry = new ExtensionEntry;
  entry->id = ext_id;
  entry->enabled = is_enabled;
  entries_.insert(std::make_pair(install_time, entry));
}
