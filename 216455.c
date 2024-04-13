bool WebContents::IsWebContentsCreationOverridden(
    content::SiteInstance* source_site_instance,
    content::mojom::WindowContainerType window_container_type,
    const GURL& opener_url,
    const std::string& frame_name,
    const GURL& target_url) {
  if (Emit("-will-add-new-contents", target_url, frame_name)) {
    return true;
  }
  return false;
}