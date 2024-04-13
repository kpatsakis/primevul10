void WebContents::CloseDevTools() {
  if (type_ == Type::REMOTE)
    return;

  managed_web_contents()->CloseDevTools();
}