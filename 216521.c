void WebContents::RenderProcessGone(base::TerminationStatus status) {
  Emit("crashed", status == base::TERMINATION_STATUS_PROCESS_WAS_KILLED);
  v8::HandleScope handle_scope(isolate());
  gin_helper::Dictionary details =
      gin_helper::Dictionary::CreateEmpty(isolate());
  details.Set("reason", status);
  Emit("render-process-gone", details);
}