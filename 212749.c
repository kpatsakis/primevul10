TEST_P(JSITest, PreparedJavaScriptURLInBacktrace) {
  std::string sourceURL = "//PreparedJavaScriptURLInBacktrace/Test/URL";
  std::string throwingSource =
      "function thrower() { throw new Error('oops')}"
      "thrower();";
  auto prep = rt.prepareJavaScript(
      std::make_unique<StringBuffer>(throwingSource), sourceURL);
  try {
    rt.evaluatePreparedJavaScript(prep);
    FAIL() << "prepareJavaScript should have thrown an exception";
  } catch (facebook::jsi::JSError err) {
    EXPECT_NE(std::string::npos, err.getStack().find(sourceURL))
        << "Backtrace should contain source URL";
  }
}