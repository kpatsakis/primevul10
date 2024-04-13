WebContents::WebContents(v8::Isolate* isolate,
                         content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      type_(Type::REMOTE),
      weak_factory_(this) {
  auto session = Session::CreateFrom(isolate, GetBrowserContext());
  session_.Reset(isolate, session.ToV8());

  web_contents->SetUserAgentOverride(blink::UserAgentOverride::UserAgentOnly(
                                         GetBrowserContext()->GetUserAgent()),
                                     false);
  Init(isolate);
  AttachAsUserData(web_contents);
  InitZoomController(web_contents, gin::Dictionary::CreateEmpty(isolate));
#if BUILDFLAG(ENABLE_ELECTRON_EXTENSIONS)
  extensions::ElectronExtensionWebContentsObserver::CreateForWebContents(
      web_contents);
  script_executor_.reset(new extensions::ScriptExecutor(web_contents));
#endif
  registry_.AddInterface(base::BindRepeating(&WebContents::BindElectronBrowser,
                                             base::Unretained(this)));
  receivers_.set_disconnect_handler(base::BindRepeating(
      &WebContents::OnElectronBrowserConnectionError, base::Unretained(this)));
}