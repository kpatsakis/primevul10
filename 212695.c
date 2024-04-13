TEST_P(JSITest, PreparedJavaScriptSourceTest) {
  rt.evaluateJavaScript(std::make_unique<StringBuffer>("var q = 0;"), "");
  auto prep = rt.prepareJavaScript(std::make_unique<StringBuffer>("q++;"), "");
  EXPECT_EQ(rt.global().getProperty(rt, "q").getNumber(), 0);
  rt.evaluatePreparedJavaScript(prep);
  EXPECT_EQ(rt.global().getProperty(rt, "q").getNumber(), 1);
  rt.evaluatePreparedJavaScript(prep);
  EXPECT_EQ(rt.global().getProperty(rt, "q").getNumber(), 2);
}