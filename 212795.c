TEST_P(JSITest, JSErrorStackOverflowHandling) {
  rt.global().setProperty(
      rt,
      "callSomething",
      Function::createFromHostFunction(
          rt,
          PropNameID::forAscii(rt, "callSomething"),
          0,
          [this](
              Runtime& rt2,
              const Value& thisVal,
              const Value* args,
              size_t count) {
            EXPECT_EQ(&rt, &rt2);
            return function("function() { return 0; }").call(rt);
          }));
  try {
    eval("(function f() { callSomething(); f.apply(); })()");
    FAIL();
  } catch (const JSError& ex) {
    EXPECT_NE(std::string(ex.what()).find("exceeded"), std::string::npos);
  }
}