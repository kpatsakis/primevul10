TEST_P(JSITest, JSErrorDoesNotInfinitelyRecurse) {
  Value globalError = rt.global().getProperty(rt, "Error");
  rt.global().setProperty(rt, "Error", Value::undefined());
  try {
    rt.global().getPropertyAsFunction(rt, "NotAFunction");
    FAIL() << "expected exception";
  } catch (const JSError& ex) {
    EXPECT_EQ(
        ex.getMessage(),
        "callGlobalFunction: JS global property 'Error' is undefined, "
        "expected a Function (while raising getPropertyAsObject: "
        "property 'NotAFunction' is undefined, expected an Object)");
  }

  // If Error is missing, this is fundamentally a problem with JS code
  // messing up the global object, so it should present in JS code as
  // a catchable string.  Not an Error (because that's broken), or as
  // a C++ failure.

  auto fails = [](Runtime& rt, const Value&, const Value*, size_t) -> Value {
    return rt.global().getPropertyAsObject(rt, "NotAProperty");
  };
  EXPECT_EQ(
      function("function (f) { try { f(); return 'undefined'; }"
               "catch (e) { return typeof e; } }")
          .call(
              rt,
              Function::createFromHostFunction(
                  rt, PropNameID::forAscii(rt, "fails"), 0, fails))
          .getString(rt)
          .utf8(rt),
      "string");

  rt.global().setProperty(rt, "Error", globalError);
}