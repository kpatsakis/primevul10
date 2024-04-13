void WebContents::OpenDevTools(gin_helper::Arguments* args) {
  if (type_ == Type::REMOTE)
    return;

  if (!enable_devtools_)
    return;

  std::string state;
  if (type_ == Type::WEB_VIEW || !owner_window()) {
    state = "detach";
  }
  bool activate = true;
  if (args && args->Length() == 1) {
    gin_helper::Dictionary options;
    if (args->GetNext(&options)) {
      options.Get("mode", &state);
      options.Get("activate", &activate);
    }
  }
  managed_web_contents()->SetDockState(state);
  managed_web_contents()->ShowDevTools(activate);
}