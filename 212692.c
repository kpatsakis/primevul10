TEST_P(JSITest, EqualsTest) {
  EXPECT_TRUE(Object::strictEquals(rt, rt.global(), rt.global()));
  EXPECT_TRUE(Value::strictEquals(rt, 1, 1));
  EXPECT_FALSE(Value::strictEquals(rt, true, 1));
  EXPECT_FALSE(Value::strictEquals(rt, true, false));
  EXPECT_TRUE(Value::strictEquals(rt, false, false));
  EXPECT_FALSE(Value::strictEquals(rt, nullptr, 1));
  EXPECT_TRUE(Value::strictEquals(rt, nullptr, nullptr));
  EXPECT_TRUE(Value::strictEquals(rt, Value::undefined(), Value()));
  EXPECT_TRUE(Value::strictEquals(rt, rt.global(), Value(rt.global())));
  EXPECT_FALSE(Value::strictEquals(
      rt,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN()));
  EXPECT_FALSE(Value::strictEquals(
      rt,
      std::numeric_limits<double>::signaling_NaN(),
      std::numeric_limits<double>::signaling_NaN()));
  EXPECT_TRUE(Value::strictEquals(rt, +0.0, -0.0));
  EXPECT_TRUE(Value::strictEquals(rt, -0.0, +0.0));

  Function noop = Function::createFromHostFunction(
      rt,
      PropNameID::forAscii(rt, "noop"),
      0,
      [](const Runtime&, const Value&, const Value*, size_t) {
        return Value();
      });
  auto noopDup = Value(rt, noop).getObject(rt);
  EXPECT_TRUE(Object::strictEquals(rt, noop, noopDup));
  EXPECT_TRUE(Object::strictEquals(rt, noopDup, noop));
  EXPECT_FALSE(Object::strictEquals(rt, noop, rt.global()));
  EXPECT_TRUE(Object::strictEquals(rt, noop, noop));
  EXPECT_TRUE(Value::strictEquals(rt, Value(rt, noop), Value(rt, noop)));

  String str = String::createFromAscii(rt, "rick");
  String strDup = String::createFromAscii(rt, "rick");
  String otherStr = String::createFromAscii(rt, "morty");
  EXPECT_TRUE(String::strictEquals(rt, str, str));
  EXPECT_TRUE(String::strictEquals(rt, str, strDup));
  EXPECT_TRUE(String::strictEquals(rt, strDup, str));
  EXPECT_FALSE(String::strictEquals(rt, str, otherStr));
  EXPECT_TRUE(Value::strictEquals(rt, Value(rt, str), Value(rt, str)));
  EXPECT_FALSE(Value::strictEquals(rt, Value(rt, str), Value(rt, noop)));
  EXPECT_FALSE(Value::strictEquals(rt, Value(rt, str), 1.0));
}