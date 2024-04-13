void WebContents::RenderViewDeleted(content::RenderViewHost* render_view_host) {
  // This event is necessary for tracking any states with respect to
  // intermediate render view hosts aka speculative render view hosts. Currently
  // used by object-registry.js to ref count remote objects.
  Emit("render-view-deleted", render_view_host->GetProcess()->GetID());

  if (-1 == currently_committed_process_id_ ||
      render_view_host->GetProcess()->GetID() ==
          currently_committed_process_id_) {
    currently_committed_process_id_ = -1;

    // When the RVH that has been deleted is the current RVH it means that the
    // the web contents are being closed. This is communicated by this event.
    // Currently tracked by guest-window-manager.js to destroy the
    // BrowserWindow.
    Emit("current-render-view-deleted",
         render_view_host->GetProcess()->GetID());
  }
}