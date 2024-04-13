bool TranslateManager::IsAcceptLanguage(TabContents* tab,
                                        const std::string& language) {
  PrefService* pref_service = tab->profile()->GetPrefs();
  PrefServiceLanguagesMap::const_iterator iter =
      accept_languages_.find(pref_service);
  if (iter == accept_languages_.end()) {
    InitAcceptLanguages(pref_service);
    notification_registrar_.Add(this, NotificationType::PROFILE_DESTROYED,
                                Source<Profile>(tab->profile()));
    pref_change_registrar_.Add(prefs::kAcceptLanguages, this);

    iter = accept_languages_.find(pref_service);
  }

  return iter->second.count(language) != 0;
}
