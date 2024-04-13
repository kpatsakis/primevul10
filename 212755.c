TEST_P(JSITest, JSErrorsArePropagatedNicely) {
  unsigned callsBeforeError = 5;

  Function sometimesThrows = function(
      "function sometimesThrows(shouldThrow, callback) {"
      "  if (shouldThrow) {"
      "    throw Error('Omg, what a nasty exception')"
      "  }"
      "  callback(callback);"
      "}");

  Function callback = Function::createFromHostFunction(
      rt,
      PropNameID::forAscii(rt, "callback"),
      0,
      [&sometimesThrows, &callsBeforeError](
          Runtime& rt, const Value& thisVal, const Value* args, size_t count) {
        return sometimesThrows.call(rt, --callsBeforeError == 0, args[0]);
      });

  try {
    sometimesThrows.call(rt, false, callback);
  } catch (JSError& error) {
    EXPECT_EQ(error.getMessage(), "Omg, what a nasty exception");
    EXPECT_EQ(countOccurences("sometimesThrows", error.getStack()), 6);

    // system JSC JSI does not implement host function names
    // EXPECT_EQ(countOccurences("callback", error.getStack(rt)), 5);
  }
}