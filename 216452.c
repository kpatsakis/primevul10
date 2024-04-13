void WebContents::WebContentsCreatedWithFullParams(
    content::WebContents* source_contents,
    int opener_render_process_id,
    int opener_render_frame_id,
    const content::mojom::CreateNewWindowParams& params,
    content::WebContents* new_contents) {
  ChildWebContentsTracker::CreateForWebContents(new_contents);
  auto* tracker = ChildWebContentsTracker::FromWebContents(new_contents);
  tracker->url = params.target_url;
  tracker->frame_name = params.frame_name;
  tracker->referrer = params.referrer.To<content::Referrer>();
  tracker->raw_features = params.raw_features;
  tracker->body = params.body;
}