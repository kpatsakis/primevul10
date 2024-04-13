TEST_P(JSITest, RuntimeTest) {
  auto v = rt.evaluateJavaScript(std::make_unique<StringBuffer>("1"), "");
  EXPECT_EQ(v.getNumber(), 1);

  rt.evaluateJavaScript(std::make_unique<StringBuffer>("x = 1"), "");
  EXPECT_EQ(rt.global().getProperty(rt, "x").getNumber(), 1);
}