const OriginIdentifierValueMap* ContentSettingsStore::GetValueMap(
    const std::string& ext_id,
    ExtensionPrefsScope scope) const {
  ExtensionEntryMap::const_iterator i = FindEntry(ext_id);
  if (i == entries_.end())
    return NULL;

  switch (scope) {
    case kExtensionPrefsScopeRegular:
      return &(i->second->settings);
    case kExtensionPrefsScopeRegularOnly:
      NOTREACHED();
      return NULL;
    case kExtensionPrefsScopeIncognitoPersistent:
      return &(i->second->incognito_persistent_settings);
    case kExtensionPrefsScopeIncognitoSessionOnly:
      return &(i->second->incognito_session_only_settings);
  }

  NOTREACHED();
  return NULL;
}
