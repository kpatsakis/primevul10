void WebContents::CloseContents(content::WebContents* source) {
  Emit("close");

  auto* autofill_driver_factory =
      AutofillDriverFactory::FromWebContents(web_contents());
  if (autofill_driver_factory) {
    autofill_driver_factory->CloseAllPopups();
  }

  if (managed_web_contents())
    managed_web_contents()->GetView()->SetDelegate(nullptr);
  for (ExtendedWebContentsObserver& observer : observers_)
    observer.OnCloseContents();
}