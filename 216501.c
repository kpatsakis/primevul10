bool WebContents::IsGuest() const {
  return type_ == Type::WEB_VIEW;
}