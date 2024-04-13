void WebContents::OnEnterFullscreenModeForTab(
    content::RenderFrameHost* requesting_frame,
    const blink::mojom::FullscreenOptions& options,
    bool allowed) {
  if (!allowed)
    return;
  CommonWebContentsDelegate::EnterFullscreenModeForTab(requesting_frame,
                                                       options);
  Emit("enter-html-full-screen");
}