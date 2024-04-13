base::ProcessId WebContents::GetOSProcessIdForFrame(
    const std::string& name,
    const std::string& document_url) const {
  for (auto* frame : web_contents()->GetAllFrames()) {
    if (frame->GetFrameName() == name &&
        frame->GetLastCommittedURL().spec() == document_url) {
      return base::GetProcId(frame->GetProcess()->GetProcess().Handle());
    }
  }
  return base::kNullProcessId;
}