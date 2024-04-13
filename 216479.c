void WebContents::DereferenceRemoteJSObject(const std::string& context_id,
                                            int object_id,
                                            int ref_count) {
  base::ListValue args;
  args.Append(context_id);
  args.Append(object_id);
  args.Append(ref_count);
  EmitWithSender("-ipc-message", receivers_.current_context(), InvokeCallback(),
                 /* internal */ true, "ELECTRON_BROWSER_DEREFERENCE",
                 std::move(args));
}