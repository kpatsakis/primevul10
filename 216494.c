v8::Local<v8::Promise> WebContents::TakeHeapSnapshot(
    const base::FilePath& file_path) {
  gin_helper::Promise<void> promise(isolate());
  v8::Local<v8::Promise> handle = promise.GetHandle();

  base::ThreadRestrictions::ScopedAllowIO allow_io;
  base::File file(file_path,
                  base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);
  if (!file.IsValid()) {
    promise.RejectWithErrorMessage("takeHeapSnapshot failed");
    return handle;
  }

  auto* frame_host = web_contents()->GetMainFrame();
  if (!frame_host) {
    promise.RejectWithErrorMessage("takeHeapSnapshot failed");
    return handle;
  }

  // This dance with `base::Owned` is to ensure that the interface stays alive
  // until the callback is called. Otherwise it would be closed at the end of
  // this function.
  auto electron_renderer =
      std::make_unique<mojo::AssociatedRemote<mojom::ElectronRenderer>>();
  frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
      electron_renderer.get());
  auto* raw_ptr = electron_renderer.get();
  (*raw_ptr)->TakeHeapSnapshot(
      mojo::WrapPlatformFile(file.TakePlatformFile()),
      base::BindOnce(
          [](mojo::AssociatedRemote<mojom::ElectronRenderer>* ep,
             gin_helper::Promise<void> promise, bool success) {
            if (success) {
              promise.Resolve();
            } else {
              promise.RejectWithErrorMessage("takeHeapSnapshot failed");
            }
          },
          base::Owned(std::move(electron_renderer)), std::move(promise)));
  return handle;
}