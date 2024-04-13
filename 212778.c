TEST_P(JSITest, FunctionConstructorTest) {
  Function ctor = function(
      "function (a) {"
      "  if (typeof a !== 'undefined') {"
      "   this.pika = a;"
      "  }"
      "}");
  ctor.getProperty(rt, "prototype")
      .getObject(rt)
      .setProperty(rt, "pika", "chu");
  auto empty = ctor.callAsConstructor(rt);
  EXPECT_TRUE(empty.isObject());
  auto emptyObj = std::move(empty).getObject(rt);
  EXPECT_EQ(emptyObj.getProperty(rt, "pika").getString(rt).utf8(rt), "chu");
  auto who = ctor.callAsConstructor(rt, "who");
  EXPECT_TRUE(who.isObject());
  auto whoObj = std::move(who).getObject(rt);
  EXPECT_EQ(whoObj.getProperty(rt, "pika").getString(rt).utf8(rt), "who");

  auto instanceof = function("function (o, b) { return o instanceof b; }");
  EXPECT_TRUE(instanceof.call(rt, emptyObj, ctor).getBool());
  EXPECT_TRUE(instanceof.call(rt, whoObj, ctor).getBool());

  auto dateCtor = rt.global().getPropertyAsFunction(rt, "Date");
  auto date = dateCtor.callAsConstructor(rt);
  EXPECT_TRUE(date.isObject());
  EXPECT_TRUE(instanceof.call(rt, date, dateCtor).getBool());
  // Sleep for 50 milliseconds
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_GE(
      function("function (d) { return (new Date()).getTime() - d.getTime(); }")
          .call(rt, date)
          .getNumber(),
      50);
}