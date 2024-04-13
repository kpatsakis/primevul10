void SearchEngineTabHelper::OnPageHasOSDD(
    const GURL& page_url,
    const GURL& osdd_url,
    const search_provider::OSDDType& msg_provider_type) {

  if (!osdd_url.is_valid() || !osdd_url.SchemeIsHTTPOrHTTPS())
    return;

  Profile* profile =
      Profile::FromBrowserContext(web_contents()->GetBrowserContext());
  if (page_url != web_contents()->GetLastCommittedURL() ||
      !TemplateURLFetcherFactory::GetForProfile(profile) ||
      profile->IsOffTheRecord())
    return;

  TemplateURLFetcher::ProviderType provider_type =
      (msg_provider_type == search_provider::AUTODETECTED_PROVIDER) ?
          TemplateURLFetcher::AUTODETECTED_PROVIDER :
          TemplateURLFetcher::EXPLICIT_PROVIDER;

  const NavigationController& controller = web_contents()->GetController();
  const NavigationEntry* entry = controller.GetLastCommittedEntry();
  for (int index = controller.GetLastCommittedEntryIndex();
       (index > 0) && IsFormSubmit(entry);
       entry = controller.GetEntryAtIndex(index))
    --index;
  if (!entry || IsFormSubmit(entry))
    return;

  base::string16 keyword;
  if (provider_type == TemplateURLFetcher::AUTODETECTED_PROVIDER) {
    keyword = GenerateKeywordFromNavigationEntry(
        entry, profile->GetPrefs()->GetString(prefs::kAcceptLanguages));
    if (keyword.empty())
      return;
  }

  TemplateURLFetcherFactory::GetForProfile(profile)->ScheduleDownload(
      keyword, osdd_url, entry->GetFavicon().url,
      base::Bind(&AssociateURLFetcherWithWebContents, web_contents()),
      base::Bind(&SearchEngineTabHelper::OnDownloadedOSDD,
                 weak_ptr_factory_.GetWeakPtr()),
      provider_type);
}
