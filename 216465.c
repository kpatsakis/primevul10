void WebContents::StartDrag(const gin_helper::Dictionary& item,
                            gin_helper::Arguments* args) {
  base::FilePath file;
  std::vector<base::FilePath> files;
  if (!item.Get("files", &files) && item.Get("file", &file)) {
    files.push_back(file);
  }

  gin::Handle<NativeImage> icon;
  if (!item.Get("icon", &icon) || icon->image().IsEmpty()) {
    args->ThrowError("Must specify non-empty 'icon' option");
    return;
  }

  // Start dragging.
  if (!files.empty()) {
    base::MessageLoopCurrent::ScopedNestableTaskAllower allow;
    DragFileItems(files, icon->image(), web_contents()->GetNativeView());
  } else {
    args->ThrowError("Must specify either 'file' or 'files' option");
  }
}