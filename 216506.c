gin::Handle<WebContents> WebContents::From(v8::Isolate* isolate,
                                           content::WebContents* web_contents) {
  auto* existing = TrackableObject::FromWrappedClass(isolate, web_contents);
  if (existing)
    return gin::CreateHandle(isolate, static_cast<WebContents*>(existing));
  else
    return gin::Handle<WebContents>();
}