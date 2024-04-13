  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   electron::api::WebContents::Type val) {
    using Type = electron::api::WebContents::Type;
    std::string type;
    switch (val) {
      case Type::BACKGROUND_PAGE:
        type = "backgroundPage";
        break;
      case Type::BROWSER_WINDOW:
        type = "window";
        break;
      case Type::BROWSER_VIEW:
        type = "browserView";
        break;
      case Type::REMOTE:
        type = "remote";
        break;
      case Type::WEB_VIEW:
        type = "webview";
        break;
      case Type::OFF_SCREEN:
        type = "offscreen";
        break;
      default:
        break;
    }
    return gin::ConvertToV8(isolate, type);
  }