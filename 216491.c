void WebContents::SetDevToolsWebContents(const WebContents* devtools) {
  if (managed_web_contents())
    managed_web_contents()->SetDevToolsWebContents(devtools->web_contents());
}