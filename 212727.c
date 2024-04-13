TEST_P(JSITest, InstanceOfTest) {
  auto ctor = function("function Rick() { this.say = 'wubalubadubdub'; }");
  auto newObj = function("function (ctor) { return new ctor(); }");
  auto instance = newObj.call(rt, ctor).getObject(rt);
  EXPECT_TRUE(instance.instanceOf(rt, ctor));
  EXPECT_EQ(
      instance.getProperty(rt, "say").getString(rt).utf8(rt), "wubalubadubdub");
  EXPECT_FALSE(Object(rt).instanceOf(rt, ctor));
  EXPECT_TRUE(ctor.callAsConstructor(rt, nullptr, 0)
                  .getObject(rt)
                  .instanceOf(rt, ctor));
}