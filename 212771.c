TEST_P(JSITest, FunctionTest) {
  // test move ctor
  Function fmove = function("function() { return 1 }");
  {
    Function g = function("function() { return 2 }");
    fmove = std::move(g);
  }
  EXPECT_EQ(fmove.call(rt).getNumber(), 2);

  // This tests all the function argument converters, and all the
  // non-lvalue overloads of call().
  Function f = function(
      "function(n, b, d, df, i, s1, s2, s3, s_sun, s_bad, o, a, f, v) { "
      "return "
      "n === null && "
      "b === true && "
      "d === 3.14 && "
      "Math.abs(df - 2.71) < 0.001 && "
      "i === 17 && "
      "s1 == 's1' && "
      "s2 == 's2' && "
      "s3 == 's3' && "
      "s_sun == 's\\u2600' && "
      "typeof s_bad == 'string' && "
      "typeof o == 'object' && "
      "Array.isArray(a) && "
      "typeof f == 'function' && "
      "v == 42 }");
  EXPECT_TRUE(f.call(
                   rt,
                   nullptr,
                   true,
                   3.14,
                   2.71f,
                   17,
                   "s1",
                   String::createFromAscii(rt, "s2"),
                   std::string{"s3"},
                   std::string{u8"s\u2600"},
                   // invalid UTF8 sequence due to unexpected continuation byte
                   std::string{"s\x80"},
                   Object(rt),
                   Array(rt, 1),
                   function("function(){}"),
                   Value(42))
                  .getBool());

  // lvalue overloads of call()
  Function flv = function(
      "function(s, o, a, f, v) { return "
      "s == 's' && "
      "typeof o == 'object' && "
      "Array.isArray(a) && "
      "typeof f == 'function' && "
      "v == 42 }");

  String s = String::createFromAscii(rt, "s");
  Object o = Object(rt);
  Array a = Array(rt, 1);
  Value v = 42;
  EXPECT_TRUE(flv.call(rt, s, o, a, f, v).getBool());

  Function f1 = function("function() { return 1; }");
  Function f2 = function("function() { return 2; }");
  f2 = std::move(f1);
  EXPECT_EQ(f2.call(rt).getNumber(), 1);
}