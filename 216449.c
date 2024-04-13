gin::Handle<WebContents> WebContents::Create(
    v8::Isolate* isolate,
    const gin_helper::Dictionary& options) {
  return gin::CreateHandle(isolate, new WebContents(isolate, options));
}