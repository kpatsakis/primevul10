void WebContents::BuildPrototype(v8::Isolate* isolate,
                                 v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "WebContents"));
  gin_helper::Destroyable::MakeDestroyable(isolate, prototype);
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetMethod("getBackgroundThrottling",
                 &WebContents::GetBackgroundThrottling)
      .SetMethod("setBackgroundThrottling",
                 &WebContents::SetBackgroundThrottling)
      .SetMethod("getProcessId", &WebContents::GetProcessID)
      .SetMethod("getOSProcessId", &WebContents::GetOSProcessID)
      .SetMethod("_getOSProcessIdForFrame",
                 &WebContents::GetOSProcessIdForFrame)
      .SetMethod("equal", &WebContents::Equal)
      .SetMethod("_loadURL", &WebContents::LoadURL)
      .SetMethod("downloadURL", &WebContents::DownloadURL)
      .SetMethod("_getURL", &WebContents::GetURL)
      .SetMethod("getTitle", &WebContents::GetTitle)
      .SetMethod("isLoading", &WebContents::IsLoading)
      .SetMethod("isLoadingMainFrame", &WebContents::IsLoadingMainFrame)
      .SetMethod("isWaitingForResponse", &WebContents::IsWaitingForResponse)
      .SetMethod("_stop", &WebContents::Stop)
      .SetMethod("_goBack", &WebContents::GoBack)
      .SetMethod("_goForward", &WebContents::GoForward)
      .SetMethod("_goToOffset", &WebContents::GoToOffset)
      .SetMethod("isCrashed", &WebContents::IsCrashed)
      .SetMethod("setUserAgent", &WebContents::SetUserAgent)
      .SetMethod("getUserAgent", &WebContents::GetUserAgent)
      .SetMethod("savePage", &WebContents::SavePage)
      .SetMethod("openDevTools", &WebContents::OpenDevTools)
      .SetMethod("closeDevTools", &WebContents::CloseDevTools)
      .SetMethod("isDevToolsOpened", &WebContents::IsDevToolsOpened)
      .SetMethod("isDevToolsFocused", &WebContents::IsDevToolsFocused)
      .SetMethod("enableDeviceEmulation", &WebContents::EnableDeviceEmulation)
      .SetMethod("disableDeviceEmulation", &WebContents::DisableDeviceEmulation)
      .SetMethod("toggleDevTools", &WebContents::ToggleDevTools)
      .SetMethod("inspectElement", &WebContents::InspectElement)
      .SetMethod("setIgnoreMenuShortcuts", &WebContents::SetIgnoreMenuShortcuts)
      .SetMethod("setAudioMuted", &WebContents::SetAudioMuted)
      .SetMethod("isAudioMuted", &WebContents::IsAudioMuted)
      .SetMethod("isCurrentlyAudible", &WebContents::IsCurrentlyAudible)
      .SetMethod("undo", &WebContents::Undo)
      .SetMethod("redo", &WebContents::Redo)
      .SetMethod("cut", &WebContents::Cut)
      .SetMethod("copy", &WebContents::Copy)
      .SetMethod("paste", &WebContents::Paste)
      .SetMethod("pasteAndMatchStyle", &WebContents::PasteAndMatchStyle)
      .SetMethod("delete", &WebContents::Delete)
      .SetMethod("selectAll", &WebContents::SelectAll)
      .SetMethod("unselect", &WebContents::Unselect)
      .SetMethod("replace", &WebContents::Replace)
      .SetMethod("replaceMisspelling", &WebContents::ReplaceMisspelling)
      .SetMethod("findInPage", &WebContents::FindInPage)
      .SetMethod("stopFindInPage", &WebContents::StopFindInPage)
      .SetMethod("focus", &WebContents::Focus)
      .SetMethod("isFocused", &WebContents::IsFocused)
      .SetMethod("tabTraverse", &WebContents::TabTraverse)
      .SetMethod("_send", &WebContents::SendIPCMessage)
      .SetMethod("_postMessage", &WebContents::PostMessage)
      .SetMethod("_sendToFrame", &WebContents::SendIPCMessageToFrame)
      .SetMethod("sendInputEvent", &WebContents::SendInputEvent)
      .SetMethod("beginFrameSubscription", &WebContents::BeginFrameSubscription)
      .SetMethod("endFrameSubscription", &WebContents::EndFrameSubscription)
      .SetMethod("startDrag", &WebContents::StartDrag)
      .SetMethod("attachToIframe", &WebContents::AttachToIframe)
      .SetMethod("detachFromOuterFrame", &WebContents::DetachFromOuterFrame)
      .SetMethod("isOffscreen", &WebContents::IsOffScreen)
#if BUILDFLAG(ENABLE_OSR)
      .SetMethod("startPainting", &WebContents::StartPainting)
      .SetMethod("stopPainting", &WebContents::StopPainting)
      .SetMethod("isPainting", &WebContents::IsPainting)
      .SetMethod("setFrameRate", &WebContents::SetFrameRate)
      .SetMethod("getFrameRate", &WebContents::GetFrameRate)
#endif
      .SetMethod("invalidate", &WebContents::Invalidate)
      .SetMethod("setZoomLevel", &WebContents::SetZoomLevel)
      .SetMethod("getZoomLevel", &WebContents::GetZoomLevel)
      .SetMethod("setZoomFactor", &WebContents::SetZoomFactor)
      .SetMethod("getZoomFactor", &WebContents::GetZoomFactor)
      .SetMethod("getType", &WebContents::GetType)
      .SetMethod("_getPreloadPaths", &WebContents::GetPreloadPaths)
      .SetMethod("getWebPreferences", &WebContents::GetWebPreferences)
      .SetMethod("getLastWebPreferences", &WebContents::GetLastWebPreferences)
      .SetMethod("getOwnerBrowserWindow", &WebContents::GetOwnerBrowserWindow)
      .SetMethod("inspectServiceWorker", &WebContents::InspectServiceWorker)
      .SetMethod("inspectSharedWorker", &WebContents::InspectSharedWorker)
      .SetMethod("inspectSharedWorkerById",
                 &WebContents::InspectSharedWorkerById)
      .SetMethod("getAllSharedWorkers", &WebContents::GetAllSharedWorkers)
#if BUILDFLAG(ENABLE_PRINTING)
      .SetMethod("_print", &WebContents::Print)
      .SetMethod("_getPrinters", &WebContents::GetPrinterList)
      .SetMethod("_printToPDF", &WebContents::PrintToPDF)
#endif
      .SetMethod("addWorkSpace", &WebContents::AddWorkSpace)
      .SetMethod("removeWorkSpace", &WebContents::RemoveWorkSpace)
      .SetMethod("showDefinitionForSelection",
                 &WebContents::ShowDefinitionForSelection)
      .SetMethod("copyImageAt", &WebContents::CopyImageAt)
      .SetMethod("capturePage", &WebContents::CapturePage)
      .SetMethod("setEmbedder", &WebContents::SetEmbedder)
      .SetMethod("setDevToolsWebContents", &WebContents::SetDevToolsWebContents)
      .SetMethod("getNativeView", &WebContents::GetNativeView)
      .SetMethod("incrementCapturerCount", &WebContents::IncrementCapturerCount)
      .SetMethod("decrementCapturerCount", &WebContents::DecrementCapturerCount)
      .SetMethod("isBeingCaptured", &WebContents::IsBeingCaptured)
      .SetMethod("setWebRTCIPHandlingPolicy",
                 &WebContents::SetWebRTCIPHandlingPolicy)
      .SetMethod("getWebRTCIPHandlingPolicy",
                 &WebContents::GetWebRTCIPHandlingPolicy)
      .SetMethod("_grantOriginAccess", &WebContents::GrantOriginAccess)
      .SetMethod("takeHeapSnapshot", &WebContents::TakeHeapSnapshot)
      .SetProperty("id", &WebContents::ID)
      .SetProperty("session", &WebContents::Session)
      .SetProperty("hostWebContents", &WebContents::HostWebContents)
      .SetProperty("devToolsWebContents", &WebContents::DevToolsWebContents)
      .SetProperty("debugger", &WebContents::Debugger);
}