base::ListValue* ContentSettingsStore::GetSettingsForExtension(
    const std::string& extension_id,
    ExtensionPrefsScope scope) const {
  base::AutoLock lock(lock_);
  const OriginIdentifierValueMap* map = GetValueMap(extension_id, scope);
  if (!map)
    return NULL;
  base::ListValue* settings = new base::ListValue();
  OriginIdentifierValueMap::EntryMap::const_iterator it;
  for (it = map->begin(); it != map->end(); ++it) {
    scoped_ptr<RuleIterator> rule_iterator(
        map->GetRuleIterator(it->first.content_type,
                             it->first.resource_identifier,
                             NULL));  // We already hold the lock.
    while (rule_iterator->HasNext()) {
      const Rule& rule = rule_iterator->Next();
      base::DictionaryValue* setting_dict = new base::DictionaryValue();
      setting_dict->SetString(keys::kPrimaryPatternKey,
                              rule.primary_pattern.ToString());
      setting_dict->SetString(keys::kSecondaryPatternKey,
                              rule.secondary_pattern.ToString());
      setting_dict->SetString(
          keys::kContentSettingsTypeKey,
          helpers::ContentSettingsTypeToString(it->first.content_type));
      setting_dict->SetString(keys::kResourceIdentifierKey,
                              it->first.resource_identifier);
      ContentSetting content_setting = ValueToContentSetting(rule.value.get());
      DCHECK_NE(CONTENT_SETTING_DEFAULT, content_setting);
      setting_dict->SetString(
          keys::kContentSettingKey,
          helpers::ContentSettingToString(content_setting));
      settings->Append(setting_dict);
    }
  }
  return settings;
}
