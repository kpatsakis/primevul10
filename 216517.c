void WebContents::InitWithSessionAndOptions(
    v8::Isolate* isolate,
    std::unique_ptr<content::WebContents> owned_web_contents,
    gin::Handle<api::Session> session,
    const gin_helper::Dictionary& options) {
  Observe(owned_web_contents.get());
  // TODO(zcbenz): Make InitWithWebContents take unique_ptr.
  // At the time of writing we are going through a refactoring and I don't want
  // to make other people's work harder.
  InitWithWebContents(owned_web_contents.release(), session->browser_context(),
                      IsGuest());

  managed_web_contents()->GetView()->SetDelegate(this);

  auto* prefs = web_contents()->GetMutableRendererPrefs();

  // Collect preferred languages from OS and browser process. accept_languages
  // effects HTTP header, navigator.languages, and CJK fallback font selection.
  //
  // Note that an application locale set to the browser process might be
  // different with the one set to the preference list.
  // (e.g. overridden with --lang)
  std::string accept_languages =
      g_browser_process->GetApplicationLocale() + ",";
  for (auto const& language : electron::GetPreferredLanguages()) {
    if (language == g_browser_process->GetApplicationLocale())
      continue;
    accept_languages += language + ",";
  }
  accept_languages.pop_back();
  prefs->accept_languages = accept_languages;

#if defined(OS_LINUX) || defined(OS_WIN)
  // Update font settings.
  static const base::NoDestructor<gfx::FontRenderParams> params(
      gfx::GetFontRenderParams(gfx::FontRenderParamsQuery(), nullptr));
  prefs->should_antialias_text = params->antialiasing;
  prefs->use_subpixel_positioning = params->subpixel_positioning;
  prefs->hinting = params->hinting;
  prefs->use_autohinter = params->autohinter;
  prefs->use_bitmaps = params->use_bitmaps;
  prefs->subpixel_rendering = params->subpixel_rendering;
#endif

  // Honor the system's cursor blink rate settings
  if (auto interval = GetCursorBlinkInterval())
    prefs->caret_blink_interval = *interval;

  // Save the preferences in C++.
  new WebContentsPreferences(web_contents(), options);

  WebContentsPermissionHelper::CreateForWebContents(web_contents());
  SecurityStateTabHelper::CreateForWebContents(web_contents());
  InitZoomController(web_contents(), options);
#if BUILDFLAG(ENABLE_ELECTRON_EXTENSIONS)
  extensions::ElectronExtensionWebContentsObserver::CreateForWebContents(
      web_contents());
  script_executor_.reset(new extensions::ScriptExecutor(web_contents()));
#endif

  registry_.AddInterface(base::BindRepeating(&WebContents::BindElectronBrowser,
                                             base::Unretained(this)));
  receivers_.set_disconnect_handler(base::BindRepeating(
      &WebContents::OnElectronBrowserConnectionError, base::Unretained(this)));
  AutofillDriverFactory::CreateForWebContents(web_contents());

  web_contents()->SetUserAgentOverride(blink::UserAgentOverride::UserAgentOnly(
                                           GetBrowserContext()->GetUserAgent()),
                                       false);

  if (IsGuest()) {
    NativeWindow* owner_window = nullptr;
    if (embedder_) {
      // New WebContents's owner_window is the embedder's owner_window.
      auto* relay =
          NativeWindowRelay::FromWebContents(embedder_->web_contents());
      if (relay)
        owner_window = relay->GetNativeWindow();
    }
    if (owner_window)
      SetOwnerWindow(owner_window);
  }

  Init(isolate);
  AttachAsUserData(web_contents());
}