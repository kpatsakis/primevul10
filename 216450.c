gin::Handle<WebContents> WebContents::CreateAndTake(
    v8::Isolate* isolate,
    std::unique_ptr<content::WebContents> web_contents,
    Type type) {
  return gin::CreateHandle(
      isolate, new WebContents(isolate, std::move(web_contents), type));
}