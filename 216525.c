void WebContents::BeforeUnloadFired(content::WebContents* tab,
                                    bool proceed,
                                    bool* proceed_to_fire_unload) {
  if (type_ == Type::BROWSER_WINDOW || type_ == Type::OFF_SCREEN)
    *proceed_to_fire_unload = proceed;
  else
    *proceed_to_fire_unload = true;
  // Note that Chromium does not emit this for navigations.
  Emit("before-unload-fired", proceed);
}