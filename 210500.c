RuleIterator* ContentSettingsStore::GetRuleIterator(
    ContentSettingsType type,
    const content_settings::ResourceIdentifier& identifier,
    bool incognito) const {
  ScopedVector<RuleIterator> iterators;
  ExtensionEntryMap::const_reverse_iterator entry;

  scoped_ptr<base::AutoLock> auto_lock(new base::AutoLock(lock_));

  for (entry = entries_.rbegin(); entry != entries_.rend(); ++entry) {
    if (!entry->second->enabled)
      continue;

    if (incognito) {
      iterators.push_back(
          entry->second->incognito_session_only_settings.GetRuleIterator(
              type,
              identifier,
              NULL));
      iterators.push_back(
          entry->second->incognito_persistent_settings.GetRuleIterator(
              type,
              identifier,
              NULL));
    } else {
      iterators.push_back(
          entry->second->settings.GetRuleIterator(type, identifier, NULL));
    }
  }
  return new ConcatenationIterator(&iterators, auto_lock.release());
}
