TEST_P(JSITest, FunctionThisTest) {
  Function checkPropertyFunction =
      function("function() { return this.a === 'a_property' }");

  Object jsObject = Object(rt);
  jsObject.setProperty(rt, "a", String::createFromUtf8(rt, "a_property"));

  class APropertyHostObject : public HostObject {
    Value get(Runtime& rt, const PropNameID& sym) override {
      return String::createFromUtf8(rt, "a_property");
    }

    void set(Runtime&, const PropNameID&, const Value&) override {}
  };
  Object hostObject =
      Object::createFromHostObject(rt, std::make_shared<APropertyHostObject>());

  EXPECT_TRUE(checkPropertyFunction.callWithThis(rt, jsObject).getBool());
  EXPECT_TRUE(checkPropertyFunction.callWithThis(rt, hostObject).getBool());
  EXPECT_FALSE(checkPropertyFunction.callWithThis(rt, Array(rt, 5)).getBool());
  EXPECT_FALSE(checkPropertyFunction.call(rt).getBool());
}