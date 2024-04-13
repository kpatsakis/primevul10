WebContents::~WebContents() {
  // The destroy() is called.
  if (managed_web_contents()) {
    managed_web_contents()->GetView()->SetDelegate(nullptr);

    RenderViewDeleted(web_contents()->GetRenderViewHost());

    if (type_ == Type::BROWSER_WINDOW && owner_window()) {
      // For BrowserWindow we should close the window and clean up everything
      // before WebContents is destroyed.
      for (ExtendedWebContentsObserver& observer : observers_)
        observer.OnCloseContents();
      // BrowserWindow destroys WebContents asynchronously, manually emit the
      // destroyed event here.
      WebContentsDestroyed();
    } else if (Browser::Get()->is_shutting_down()) {
      // Destroy WebContents directly when app is shutting down.
      DestroyWebContents(false /* async */);
    } else {
      // Destroy WebContents asynchronously unless app is shutting down,
      // because destroy() might be called inside WebContents's event handler.
      DestroyWebContents(!IsGuest() /* async */);
      // The WebContentsDestroyed will not be called automatically because we
      // destroy the webContents in the next tick. So we have to manually
      // call it here to make sure "destroyed" event is emitted.
      WebContentsDestroyed();
    }
  }
}