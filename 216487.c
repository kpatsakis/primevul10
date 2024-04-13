bool WebContents::IsDevToolsOpened() {
  if (type_ == Type::REMOTE)
    return false;

  return managed_web_contents()->IsDevToolsViewShowing();
}