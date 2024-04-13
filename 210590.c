void SearchEngineTabHelper::DidNavigateMainFrame(
    const content::LoadCommittedDetails& /*details*/,
    const content::FrameNavigateParams& params) {
  GenerateKeywordIfNecessary(params);
}
