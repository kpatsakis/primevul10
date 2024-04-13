void WebContents::RenderViewHostChanged(content::RenderViewHost* old_host,
                                        content::RenderViewHost* new_host) {
  currently_committed_process_id_ = new_host->GetProcess()->GetID();
}