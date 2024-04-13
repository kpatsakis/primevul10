gin::Handle<WebContents> WebContents::FromOrCreate(
    v8::Isolate* isolate,
    content::WebContents* web_contents) {
  auto existing = From(isolate, web_contents);
  if (!existing.IsEmpty())
    return existing;
  else
    return gin::CreateHandle(isolate, new WebContents(isolate, web_contents));
}