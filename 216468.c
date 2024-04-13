content::JavaScriptDialogManager* WebContents::GetJavaScriptDialogManager(
    content::WebContents* source) {
  if (!dialog_manager_)
    dialog_manager_ = std::make_unique<ElectronJavaScriptDialogManager>(this);

  return dialog_manager_.get();
}