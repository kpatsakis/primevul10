v8::Local<v8::Value> WebContents::GetOwnerBrowserWindow() const {
  if (owner_window())
    return BrowserWindow::From(isolate(), owner_window());
  else
    return v8::Null(isolate());
}