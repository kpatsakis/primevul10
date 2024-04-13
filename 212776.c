TEST_P(JSITest, JSErrorsCanBeConstructedWithStack) {
  auto err = JSError(rt, "message", "stack");
  EXPECT_EQ(err.getMessage(), "message");
  EXPECT_EQ(err.getStack(), "stack");
}