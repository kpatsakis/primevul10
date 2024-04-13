void WebContents::AddNewContents(
    content::WebContents* source,
    std::unique_ptr<content::WebContents> new_contents,
    const GURL& target_url,
    WindowOpenDisposition disposition,
    const gfx::Rect& initial_rect,
    bool user_gesture,
    bool* was_blocked) {
  auto* tracker = ChildWebContentsTracker::FromWebContents(new_contents.get());
  DCHECK(tracker);

  v8::Locker locker(isolate());
  v8::HandleScope handle_scope(isolate());
  auto api_web_contents =
      CreateAndTake(isolate(), std::move(new_contents), Type::BROWSER_WINDOW);
  if (Emit("-add-new-contents", api_web_contents, disposition, user_gesture,
           initial_rect.x(), initial_rect.y(), initial_rect.width(),
           initial_rect.height(), tracker->url, tracker->frame_name,
           tracker->referrer, tracker->raw_features, tracker->body)) {
    // TODO(zcbenz): Can we make this sync?
    api_web_contents->DestroyWebContents(true /* async */);
  }
}