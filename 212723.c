TEST_P(JSITest, ArrayTest) {
  eval("x = {1:2, '3':4, 5:'six', 'seven':['eight', 'nine']}");

  Object x = rt.global().getPropertyAsObject(rt, "x");
  Array names = x.getPropertyNames(rt);
  EXPECT_EQ(names.size(rt), 4);
  std::unordered_set<std::string> strNames;
  for (size_t i = 0; i < names.size(rt); ++i) {
    Value n = names.getValueAtIndex(rt, i);
    EXPECT_TRUE(n.isString());
    strNames.insert(n.getString(rt).utf8(rt));
  }

  EXPECT_EQ(strNames.size(), 4);
  EXPECT_EQ(strNames.count("1"), 1);
  EXPECT_EQ(strNames.count("3"), 1);
  EXPECT_EQ(strNames.count("5"), 1);
  EXPECT_EQ(strNames.count("seven"), 1);

  Object seven = x.getPropertyAsObject(rt, "seven");
  Array arr = seven.getArray(rt);

  EXPECT_EQ(arr.size(rt), 2);
  EXPECT_EQ(arr.getValueAtIndex(rt, 0).getString(rt).utf8(rt), "eight");
  EXPECT_EQ(arr.getValueAtIndex(rt, 1).getString(rt).utf8(rt), "nine");
  // TODO: test out of range

  EXPECT_EQ(x.getPropertyAsObject(rt, "seven").getArray(rt).size(rt), 2);

  // Check that property access with both symbols and strings can access
  // array values.
  EXPECT_EQ(seven.getProperty(rt, "0").getString(rt).utf8(rt), "eight");
  EXPECT_EQ(seven.getProperty(rt, "1").getString(rt).utf8(rt), "nine");
  seven.setProperty(rt, "1", "modified");
  EXPECT_EQ(seven.getProperty(rt, "1").getString(rt).utf8(rt), "modified");
  EXPECT_EQ(arr.getValueAtIndex(rt, 1).getString(rt).utf8(rt), "modified");
  EXPECT_EQ(
      seven.getProperty(rt, PropNameID::forAscii(rt, "0"))
          .getString(rt)
          .utf8(rt),
      "eight");
  seven.setProperty(rt, PropNameID::forAscii(rt, "0"), "modified2");
  EXPECT_EQ(arr.getValueAtIndex(rt, 0).getString(rt).utf8(rt), "modified2");

  Array alpha = Array(rt, 4);
  EXPECT_TRUE(alpha.getValueAtIndex(rt, 0).isUndefined());
  EXPECT_TRUE(alpha.getValueAtIndex(rt, 3).isUndefined());
  EXPECT_EQ(alpha.size(rt), 4);
  alpha.setValueAtIndex(rt, 0, "a");
  alpha.setValueAtIndex(rt, 1, "b");
  EXPECT_EQ(alpha.length(rt), 4);
  alpha.setValueAtIndex(rt, 2, "c");
  alpha.setValueAtIndex(rt, 3, "d");
  EXPECT_EQ(alpha.size(rt), 4);

  EXPECT_TRUE(
      function(
          "function (arr) { return "
          "arr.length == 4 && "
          "['a','b','c','d'].every(function(v,i) { return v === arr[i]}); }")
          .call(rt, alpha)
          .getBool());

  Array alpha2 = Array(rt, 1);
  alpha2 = std::move(alpha);
  EXPECT_EQ(alpha2.size(rt), 4);
}