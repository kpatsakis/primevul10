void WebContents::DecrementCapturerCount(gin_helper::Arguments* args) {
  bool stay_hidden = false;

  // get stayHidden arguments if they exist
  args->GetNext(&stay_hidden);

  web_contents()->DecrementCapturerCount(stay_hidden);
}