void TranslateManager::InitiateTranslation(TabContents* tab,
                                           const std::string& page_lang) {
  PrefService* prefs = tab->profile()->GetPrefs();
  if (!prefs->GetBoolean(prefs::kEnableTranslate))
    return;

  pref_change_registrar_.Init(prefs);

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kDisableTranslate))
    return;

  NavigationEntry* entry = tab->controller().GetActiveEntry();
  if (!entry) {
    return;
  }

  if (GetTranslateInfoBarDelegate(tab))
    return;

  std::string target_lang = GetTargetLanguage();
  if (target_lang.empty() || !IsSupportedLanguage(page_lang)) {
    return;
  }

  if (!IsTranslatableURL(entry->url()) || page_lang == target_lang ||
      !TranslatePrefs::CanTranslate(prefs, page_lang, entry->url()) ||
      IsAcceptLanguage(tab, page_lang)) {
    return;
  }

  std::string auto_target_lang;
  if (!tab->profile()->IsOffTheRecord() &&
      TranslatePrefs::ShouldAutoTranslate(prefs, page_lang,
          &auto_target_lang)) {
    TranslatePage(tab, page_lang, auto_target_lang);
    return;
  }

  std::string auto_translate_to = tab->language_state().AutoTranslateTo();
  if (!auto_translate_to.empty()) {
    TranslatePage(tab, page_lang, auto_translate_to);
    return;
  }

  tab->AddInfoBar(TranslateInfoBarDelegate::CreateDelegate(
      TranslateInfoBarDelegate::BEFORE_TRANSLATE, tab,
      page_lang, target_lang));
}
