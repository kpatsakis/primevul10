void WebContents::RenderFrameDeleted(
    content::RenderFrameHost* render_frame_host) {
  // A RenderFrameHost can be destroyed before the related Mojo binding is
  // closed, which can result in Mojo calls being sent for RenderFrameHosts
  // that no longer exist. To prevent this from happening, when a
  // RenderFrameHost goes away, we close all the bindings related to that
  // frame.
  auto it = frame_to_receivers_map_.find(render_frame_host);
  if (it == frame_to_receivers_map_.end())
    return;
  for (auto id : it->second)
    receivers_.Remove(id);
  frame_to_receivers_map_.erase(it);
}