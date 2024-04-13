v8::Local<v8::Promise> WebContents::PrintToPDF(base::DictionaryValue settings) {
  gin_helper::Promise<v8::Local<v8::Value>> promise(isolate());
  v8::Local<v8::Promise> handle = promise.GetHandle();
  PrintPreviewMessageHandler::FromWebContents(web_contents())
      ->PrintToPDF(std::move(settings), std::move(promise));
  return handle;
}