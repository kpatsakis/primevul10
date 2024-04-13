bool WebContents::IsDevToolsFocused() {
  if (type_ == Type::REMOTE)
    return false;

  return managed_web_contents()->GetView()->IsDevToolsViewFocused();
}