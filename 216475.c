void WebContents::IncrementCapturerCount(gin_helper::Arguments* args) {
  gfx::Size size;
  bool stay_hidden = false;

  // get size arguments if they exist
  args->GetNext(&size);
  // get stayHidden arguments if they exist
  args->GetNext(&stay_hidden);

  web_contents()->IncrementCapturerCount(size, stay_hidden);
}