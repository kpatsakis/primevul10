TEST_P(JSITest, HostObjectProtoTest) {
  class ProtoHostObject : public HostObject {
    Value get(Runtime& rt, const PropNameID&) override {
      return String::createFromAscii(rt, "phoprop");
    }
  };

  rt.global().setProperty(
      rt,
      "pho",
      Object::createFromHostObject(rt, std::make_shared<ProtoHostObject>()));

  EXPECT_EQ(
      eval("({__proto__: pho})[Symbol.toPrimitive]").getString(rt).utf8(rt),
      "phoprop");
}