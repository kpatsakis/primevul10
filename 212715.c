TEST_P(JSITest, ObjectTest) {
  eval("x = {1:2, '3':4, 5:'six', 'seven':['eight', 'nine']}");
  Object x = rt.global().getPropertyAsObject(rt, "x");
  EXPECT_EQ(x.getPropertyNames(rt).size(rt), 4);
  EXPECT_TRUE(x.hasProperty(rt, "1"));
  EXPECT_TRUE(x.hasProperty(rt, PropNameID::forAscii(rt, "1")));
  EXPECT_FALSE(x.hasProperty(rt, "2"));
  EXPECT_FALSE(x.hasProperty(rt, PropNameID::forAscii(rt, "2")));
  EXPECT_TRUE(x.hasProperty(rt, "3"));
  EXPECT_TRUE(x.hasProperty(rt, PropNameID::forAscii(rt, "3")));
  EXPECT_TRUE(x.hasProperty(rt, "seven"));
  EXPECT_TRUE(x.hasProperty(rt, PropNameID::forAscii(rt, "seven")));
  EXPECT_EQ(x.getProperty(rt, "1").getNumber(), 2);
  EXPECT_EQ(x.getProperty(rt, PropNameID::forAscii(rt, "1")).getNumber(), 2);
  EXPECT_EQ(x.getProperty(rt, "3").getNumber(), 4);
  Value five = 5;
  EXPECT_EQ(
      x.getProperty(rt, PropNameID::forString(rt, five.toString(rt)))
          .getString(rt)
          .utf8(rt),
      "six");

  x.setProperty(rt, "ten", 11);
  EXPECT_EQ(x.getPropertyNames(rt).size(rt), 5);
  EXPECT_TRUE(eval("x.ten == 11").getBool());

  x.setProperty(rt, "e_as_float", 2.71f);
  EXPECT_TRUE(eval("Math.abs(x.e_as_float - 2.71) < 0.001").getBool());

  x.setProperty(rt, "e_as_double", 2.71);
  EXPECT_TRUE(eval("x.e_as_double == 2.71").getBool());

  uint8_t utf8[] = {0xF0, 0x9F, 0x86, 0x97};
  String nonAsciiName = String::createFromUtf8(rt, utf8, sizeof(utf8));
  x.setProperty(rt, PropNameID::forString(rt, nonAsciiName), "emoji");
  EXPECT_EQ(x.getPropertyNames(rt).size(rt), 8);
  EXPECT_TRUE(eval("x['\\uD83C\\uDD97'] == 'emoji'").getBool());

  Object seven = x.getPropertyAsObject(rt, "seven");
  EXPECT_TRUE(seven.isArray(rt));
  Object evalf = rt.global().getPropertyAsObject(rt, "eval");
  EXPECT_TRUE(evalf.isFunction(rt));

  Object movedX = Object(rt);
  movedX = std::move(x);
  EXPECT_EQ(movedX.getPropertyNames(rt).size(rt), 8);
  EXPECT_EQ(movedX.getProperty(rt, "1").getNumber(), 2);

  Object obj = Object(rt);
  obj.setProperty(rt, "roses", "red");
  obj.setProperty(rt, "violets", "blue");
  Object oprop = Object(rt);
  obj.setProperty(rt, "oprop", oprop);
  obj.setProperty(rt, "aprop", Array(rt, 1));

  EXPECT_TRUE(function("function (obj) { return "
                       "obj.roses == 'red' && "
                       "obj['violets'] == 'blue' && "
                       "typeof obj.oprop == 'object' && "
                       "Array.isArray(obj.aprop); }")
                  .call(rt, obj)
                  .getBool());

  // Check that getPropertyNames doesn't return non-enumerable
  // properties.
  obj = function(
            "function () {"
            "  obj = {};"
            "  obj.a = 1;"
            "  Object.defineProperty(obj, 'b', {"
            "    enumerable: false,"
            "    value: 2"
            "  });"
            "  return obj;"
            "}")
            .call(rt)
            .getObject(rt);
  EXPECT_EQ(obj.getProperty(rt, "a").getNumber(), 1);
  EXPECT_EQ(obj.getProperty(rt, "b").getNumber(), 2);
  Array names = obj.getPropertyNames(rt);
  EXPECT_EQ(names.size(rt), 1);
  EXPECT_EQ(names.getValueAtIndex(rt, 0).getString(rt).utf8(rt), "a");
}