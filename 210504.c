void ContentSettingsStore::NotifyOfContentSettingChanged(
    const std::string& extension_id,
    bool incognito) {
  FOR_EACH_OBSERVER(
      ContentSettingsStore::Observer,
      observers_,
      OnContentSettingChanged(extension_id, incognito));
}
