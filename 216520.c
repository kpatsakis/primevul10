void WebContents::SetUserAgent(const std::string& user_agent,
                               gin_helper::Arguments* args) {
  web_contents()->SetUserAgentOverride(
      blink::UserAgentOverride::UserAgentOnly(user_agent), false);
}