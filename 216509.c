void WebContents::BeginFrameSubscription(gin_helper::Arguments* args) {
  bool only_dirty = false;
  FrameSubscriber::FrameCaptureCallback callback;

  args->GetNext(&only_dirty);
  if (!args->GetNext(&callback)) {
    args->ThrowError();
    return;
  }

  frame_subscriber_ =
      std::make_unique<FrameSubscriber>(web_contents(), callback, only_dirty);
}