void AssociateURLFetcherWithWebContents(content::WebContents* web_contents,
                                        net::URLFetcher* url_fetcher) {
  content::AssociateURLFetcherWithRenderFrame(
      url_fetcher,
      web_contents->GetURL(),
      web_contents->GetRenderProcessHost()->GetID(),
      web_contents->GetMainFrame()->GetRoutingID());
}
