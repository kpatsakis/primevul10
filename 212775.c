TEST_P(JSITest, ValueTest) {
  EXPECT_TRUE(checkValue(Value::undefined(), "undefined"));
  EXPECT_TRUE(checkValue(Value(), "undefined"));
  EXPECT_TRUE(checkValue(Value::null(), "null"));
  EXPECT_TRUE(checkValue(nullptr, "null"));

  EXPECT_TRUE(checkValue(Value(false), "false"));
  EXPECT_TRUE(checkValue(false, "false"));
  EXPECT_TRUE(checkValue(true, "true"));

  EXPECT_TRUE(checkValue(Value(1.5), "1.5"));
  EXPECT_TRUE(checkValue(2.5, "2.5"));

  EXPECT_TRUE(checkValue(Value(10), "10"));
  EXPECT_TRUE(checkValue(20, "20"));
  EXPECT_TRUE(checkValue(30, "30"));

  // rvalue implicit conversion
  EXPECT_TRUE(checkValue(String::createFromAscii(rt, "one"), "'one'"));
  // lvalue explicit copy
  String s = String::createFromAscii(rt, "two");
  EXPECT_TRUE(checkValue(Value(rt, s), "'two'"));

  {
    // rvalue assignment of trivial value
    Value v1 = 100;
    Value v2 = String::createFromAscii(rt, "hundred");
    v2 = std::move(v1);
    EXPECT_TRUE(v2.isNumber());
    EXPECT_EQ(v2.getNumber(), 100);
  }

  {
    // rvalue assignment of js heap value
    Value v1 = String::createFromAscii(rt, "hundred");
    Value v2 = 100;
    v2 = std::move(v1);
    EXPECT_TRUE(v2.isString());
    EXPECT_EQ(v2.getString(rt).utf8(rt), "hundred");
  }

  Object o = Object(rt);
  EXPECT_TRUE(function("function(value) { return typeof(value) == 'object'; }")
                  .call(rt, Value(rt, o))
                  .getBool());

  uint8_t utf8[] = "[null, 2, \"c\", \"emoji: \xf0\x9f\x86\x97\", {}]";

  EXPECT_TRUE(
      function("function (arr) { return "
               "Array.isArray(arr) && "
               "arr.length == 5 && "
               "arr[0] === null && "
               "arr[1] == 2 && "
               "arr[2] == 'c' && "
               "arr[3] == 'emoji: \\uD83C\\uDD97' && "
               "typeof arr[4] == 'object'; }")
          .call(rt, Value::createFromJsonUtf8(rt, utf8, sizeof(utf8) - 1))
          .getBool());

  EXPECT_TRUE(eval("undefined").isUndefined());
  EXPECT_TRUE(eval("null").isNull());
  EXPECT_TRUE(eval("true").isBool());
  EXPECT_TRUE(eval("false").isBool());
  EXPECT_TRUE(eval("123").isNumber());
  EXPECT_TRUE(eval("123.4").isNumber());
  EXPECT_TRUE(eval("'str'").isString());
  // "{}" returns undefined.  empty code block?
  EXPECT_TRUE(eval("({})").isObject());
  EXPECT_TRUE(eval("[]").isObject());
  EXPECT_TRUE(eval("(function(){})").isObject());

  EXPECT_EQ(eval("123").getNumber(), 123);
  EXPECT_EQ(eval("123.4").getNumber(), 123.4);
  EXPECT_EQ(eval("'str'").getString(rt).utf8(rt), "str");
  EXPECT_TRUE(eval("[]").getObject(rt).isArray(rt));

  EXPECT_EQ(eval("456").asNumber(), 456);
  EXPECT_THROW(eval("'word'").asNumber(), JSIException);
  EXPECT_EQ(
      eval("({1:2, 3:4})").asObject(rt).getProperty(rt, "1").getNumber(), 2);
  EXPECT_THROW(eval("'oops'").asObject(rt), JSIException);

  EXPECT_EQ(eval("['zero',1,2,3]").toString(rt).utf8(rt), "zero,1,2,3");
}