void SearchEngineTabHelper::OnDownloadedOSDD(
    scoped_ptr<TemplateURL> template_url) {
  Profile* profile =
      Profile::FromBrowserContext(web_contents()->GetBrowserContext());
  delegate_->ConfirmAddSearchProvider(template_url.release(), profile);
}
