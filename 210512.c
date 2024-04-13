ContentSettingsStore::~ContentSettingsStore() {
  STLDeleteValues(&entries_);
}
