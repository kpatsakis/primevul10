void WebContents::WebContentsDestroyed() {
  // Give chance for guest delegate to cleanup its observers
  // since the native class is only destroyed in the next tick.
  if (guest_delegate_)
    guest_delegate_->WillDestroy();

  // Cleanup relationships with other parts.
  RemoveFromWeakMap();

  // We can not call Destroy here because we need to call Emit first, but we
  // also do not want any method to be used, so just mark as destroyed here.
  MarkDestroyed();

  Emit("destroyed");

  // For guest view based on OOPIF, the WebContents is released by the embedder
  // frame, and we need to clear the reference to the memory.
  if (IsGuest() && managed_web_contents()) {
    managed_web_contents()->ReleaseWebContents();
    ResetManagedWebContents(false);
  }

  // Destroy the native class in next tick.
  base::ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE, GetDestroyClosure());
}