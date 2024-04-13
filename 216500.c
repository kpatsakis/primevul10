bool WebContents::HandleKeyboardEvent(
    content::WebContents* source,
    const content::NativeWebKeyboardEvent& event) {
  if (type_ == Type::WEB_VIEW && embedder_) {
    // Send the unhandled keyboard events back to the embedder.
    return embedder_->HandleKeyboardEvent(source, event);
  } else {
    // Go to the default keyboard handling.
    return CommonWebContentsDelegate::HandleKeyboardEvent(source, event);
  }
}