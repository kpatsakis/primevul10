SearchEngineTabHelper::SearchEngineTabHelper(WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      delegate_(nullptr),
      weak_ptr_factory_(this) {
  DCHECK(web_contents);
}
