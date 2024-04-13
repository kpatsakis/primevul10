TEST_P(JSITest, ExceptionStackTraceTest) {
  static const char invokeUndefinedScript[] =
      "function hello() {"
      "  var a = {}; a.log(); }"
      "function world() { hello(); }"
      "world()";
  std::string stack;
  try {
    rt.evaluateJavaScript(
        std::make_unique<StringBuffer>(invokeUndefinedScript), "");
  } catch (JSError& e) {
    stack = e.getStack();
  }
  EXPECT_NE(stack.find("world"), std::string::npos);
}