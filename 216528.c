void WebContents::ShowDefinitionForSelection() {
#if defined(OS_MACOSX)
  auto* const view = web_contents()->GetRenderWidgetHostView();
  if (view)
    view->ShowDefinitionForSelection();
#endif
}