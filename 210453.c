void TranslateManager::Observe(NotificationType type,
                               const NotificationSource& source,
                               const NotificationDetails& details) {
  switch (type.value) {
    case NotificationType::NAV_ENTRY_COMMITTED: {
      NavigationController* controller =
          Source<NavigationController>(source).ptr();
      NavigationController::LoadCommittedDetails* load_details =
          Details<NavigationController::LoadCommittedDetails>(details).ptr();
      NavigationEntry* entry = controller->GetActiveEntry();
      if (!entry) {
        NOTREACHED();
        return;
      }
      if (!load_details->is_main_frame &&
          controller->tab_contents()->language_state().translation_declined()) {
        return;
      }
      if (entry->transition_type() != PageTransition::RELOAD &&
          load_details->type != NavigationType::SAME_PAGE) {
        return;
      }
      MessageLoop::current()->PostTask(FROM_HERE,
          method_factory_.NewRunnableMethod(
              &TranslateManager::InitiateTranslationPosted,
              controller->tab_contents()->render_view_host()->process()->id(),
              controller->tab_contents()->render_view_host()->routing_id(),
              controller->tab_contents()->language_state().
                  original_language()));
      break;
    }
    case NotificationType::TAB_LANGUAGE_DETERMINED: {
      TabContents* tab = Source<TabContents>(source).ptr();
      LanguageState& language_state = tab->language_state();
      if (language_state.page_translatable() &&
          !language_state.translation_pending() &&
          !language_state.translation_declined() &&
          !language_state.IsPageTranslated()) {
        std::string language = *(Details<std::string>(details).ptr());
        InitiateTranslation(tab, language);
      }
      break;
    }
    case NotificationType::PAGE_TRANSLATED: {
      TabContents* tab = Source<TabContents>(source).ptr();
      PageTranslatedDetails* page_translated_details =
          Details<PageTranslatedDetails>(details).ptr();
      PageTranslated(tab, page_translated_details);
      break;
    }
    case NotificationType::PROFILE_DESTROYED: {
      Profile* profile = Source<Profile>(source).ptr();
      notification_registrar_.Remove(this, NotificationType::PROFILE_DESTROYED,
                                     source);
      size_t count = accept_languages_.erase(profile->GetPrefs());
      DCHECK(count > 0);
      pref_change_registrar_.Remove(prefs::kAcceptLanguages, this);
      break;
    }
    case NotificationType::PREF_CHANGED: {
      DCHECK(*Details<std::string>(details).ptr() == prefs::kAcceptLanguages);
      PrefService* prefs = Source<PrefService>(source).ptr();
      InitAcceptLanguages(prefs);
      break;
    }
    default:
      NOTREACHED();
  }
}
