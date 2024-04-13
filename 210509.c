void ContentSettingsStore::SetExtensionContentSettingFromList(
    const std::string& extension_id,
    const base::ListValue* list,
    ExtensionPrefsScope scope) {
  for (base::ListValue::const_iterator it = list->begin();
       it != list->end(); ++it) {
    if ((*it)->GetType() != Value::TYPE_DICTIONARY) {
      NOTREACHED();
      continue;
    }
    base::DictionaryValue* dict = static_cast<base::DictionaryValue*>(*it);
    std::string primary_pattern_str;
    dict->GetString(keys::kPrimaryPatternKey, &primary_pattern_str);
    ContentSettingsPattern primary_pattern =
        ContentSettingsPattern::FromString(primary_pattern_str);
    DCHECK(primary_pattern.IsValid());

    std::string secondary_pattern_str;
    dict->GetString(keys::kSecondaryPatternKey, &secondary_pattern_str);
    ContentSettingsPattern secondary_pattern =
        ContentSettingsPattern::FromString(secondary_pattern_str);
    DCHECK(secondary_pattern.IsValid());

    std::string content_settings_type_str;
    dict->GetString(keys::kContentSettingsTypeKey, &content_settings_type_str);
    ContentSettingsType content_settings_type =
        helpers::StringToContentSettingsType(content_settings_type_str);
    DCHECK_NE(CONTENT_SETTINGS_TYPE_DEFAULT, content_settings_type);

    std::string resource_identifier;
    dict->GetString(keys::kResourceIdentifierKey, &resource_identifier);

    std::string content_setting_string;
    dict->GetString(keys::kContentSettingKey, &content_setting_string);
    ContentSetting setting = CONTENT_SETTING_DEFAULT;
    bool result =
        helpers::StringToContentSetting(content_setting_string, &setting);
    DCHECK(result);

    SetExtensionContentSetting(extension_id,
                               primary_pattern,
                               secondary_pattern,
                               content_settings_type,
                               resource_identifier,
                               setting,
                               scope);
  }
}
