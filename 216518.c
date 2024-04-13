void WebContents::DidStopLoading() {
  Emit("did-stop-loading");
}