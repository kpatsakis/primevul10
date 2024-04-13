TEST_P(JSITest, HostObjectTest) {
  class ConstantHostObject : public HostObject {
    Value get(Runtime&, const PropNameID& sym) override {
      return 9000;
    }

    void set(Runtime&, const PropNameID&, const Value&) override {}
  };

  Object cho =
      Object::createFromHostObject(rt, std::make_shared<ConstantHostObject>());
  EXPECT_TRUE(function("function (obj) { return obj.someRandomProp == 9000; }")
                  .call(rt, cho)
                  .getBool());
  EXPECT_TRUE(cho.isHostObject(rt));
  EXPECT_TRUE(cho.getHostObject<ConstantHostObject>(rt).get() != nullptr);

  struct SameRuntimeHostObject : HostObject {
    SameRuntimeHostObject(Runtime& rt) : rt_(rt){};

    Value get(Runtime& rt, const PropNameID& sym) override {
      EXPECT_EQ(&rt, &rt_);
      return Value();
    }

    void set(Runtime& rt, const PropNameID& name, const Value& value) override {
      EXPECT_EQ(&rt, &rt_);
    }

    std::vector<PropNameID> getPropertyNames(Runtime& rt) override {
      EXPECT_EQ(&rt, &rt_);
      return {};
    }

    Runtime& rt_;
  };

  Object srho = Object::createFromHostObject(
      rt, std::make_shared<SameRuntimeHostObject>(rt));
  // Test get's Runtime is as expected
  function("function (obj) { return obj.isSame; }").call(rt, srho);
  // ... and set
  function("function (obj) { obj['k'] = 'v'; }").call(rt, srho);
  // ... and getPropertyNames
  function("function (obj) { for (k in obj) {} }").call(rt, srho);

  class TwiceHostObject : public HostObject {
    Value get(Runtime& rt, const PropNameID& sym) override {
      return String::createFromUtf8(rt, sym.utf8(rt) + sym.utf8(rt));
    }

    void set(Runtime&, const PropNameID&, const Value&) override {}
  };

  Object tho =
      Object::createFromHostObject(rt, std::make_shared<TwiceHostObject>());
  EXPECT_TRUE(function("function (obj) { return obj.abc == 'abcabc'; }")
                  .call(rt, tho)
                  .getBool());
  EXPECT_TRUE(function("function (obj) { return obj['def'] == 'defdef'; }")
                  .call(rt, tho)
                  .getBool());
  EXPECT_TRUE(function("function (obj) { return obj[12] === '1212'; }")
                  .call(rt, tho)
                  .getBool());
  EXPECT_TRUE(tho.isHostObject(rt));
  EXPECT_TRUE(
      std::dynamic_pointer_cast<ConstantHostObject>(tho.getHostObject(rt)) ==
      nullptr);
  EXPECT_TRUE(tho.getHostObject<TwiceHostObject>(rt).get() != nullptr);

  class PropNameIDHostObject : public HostObject {
    Value get(Runtime& rt, const PropNameID& sym) override {
      if (PropNameID::compare(rt, sym, PropNameID::forAscii(rt, "undef"))) {
        return Value::undefined();
      } else {
        return PropNameID::compare(
            rt, sym, PropNameID::forAscii(rt, "somesymbol"));
      }
    }

    void set(Runtime&, const PropNameID&, const Value&) override {}
  };

  Object sho = Object::createFromHostObject(
      rt, std::make_shared<PropNameIDHostObject>());
  EXPECT_TRUE(sho.isHostObject(rt));
  EXPECT_TRUE(function("function (obj) { return obj.undef; }")
                  .call(rt, sho)
                  .isUndefined());
  EXPECT_TRUE(function("function (obj) { return obj.somesymbol; }")
                  .call(rt, sho)
                  .getBool());
  EXPECT_FALSE(function("function (obj) { return obj.notsomuch; }")
                   .call(rt, sho)
                   .getBool());

  class BagHostObject : public HostObject {
   public:
    const std::string& getThing() {
      return bag_["thing"];
    }

   private:
    Value get(Runtime& rt, const PropNameID& sym) override {
      if (sym.utf8(rt) == "thing") {
        return String::createFromUtf8(rt, bag_[sym.utf8(rt)]);
      }
      return Value::undefined();
    }

    void set(Runtime& rt, const PropNameID& sym, const Value& val) override {
      std::string key(sym.utf8(rt));
      if (key == "thing") {
        bag_[key] = val.toString(rt).utf8(rt);
      }
    }

    std::unordered_map<std::string, std::string> bag_;
  };

  std::shared_ptr<BagHostObject> shbho = std::make_shared<BagHostObject>();
  Object bho = Object::createFromHostObject(rt, shbho);
  EXPECT_TRUE(bho.isHostObject(rt));
  EXPECT_TRUE(function("function (obj) { return obj.undef; }")
                  .call(rt, bho)
                  .isUndefined());
  EXPECT_EQ(
      function("function (obj) { obj.thing = 'hello'; return obj.thing; }")
          .call(rt, bho)
          .toString(rt)
          .utf8(rt),
      "hello");
  EXPECT_EQ(shbho->getThing(), "hello");

  class ThrowingHostObject : public HostObject {
    Value get(Runtime& rt, const PropNameID& sym) override {
      throw std::runtime_error("Cannot get");
    }

    void set(Runtime& rt, const PropNameID& sym, const Value& val) override {
      throw std::runtime_error("Cannot set");
    }
  };

  Object thro =
      Object::createFromHostObject(rt, std::make_shared<ThrowingHostObject>());
  EXPECT_TRUE(thro.isHostObject(rt));
  std::string exc;
  try {
    function("function (obj) { return obj.thing; }").call(rt, thro);
  } catch (const JSError& ex) {
    exc = ex.what();
  }
  EXPECT_NE(exc.find("Cannot get"), std::string::npos);
  exc = "";
  try {
    function("function (obj) { obj.thing = 'hello'; }").call(rt, thro);
  } catch (const JSError& ex) {
    exc = ex.what();
  }
  EXPECT_NE(exc.find("Cannot set"), std::string::npos);

  class NopHostObject : public HostObject {};
  Object nopHo =
      Object::createFromHostObject(rt, std::make_shared<NopHostObject>());
  EXPECT_TRUE(nopHo.isHostObject(rt));
  EXPECT_TRUE(function("function (obj) { return obj.thing; }")
                  .call(rt, nopHo)
                  .isUndefined());

  std::string nopExc;
  try {
    function("function (obj) { obj.thing = 'pika'; }").call(rt, nopHo);
  } catch (const JSError& ex) {
    nopExc = ex.what();
  }
  EXPECT_NE(nopExc.find("TypeError: "), std::string::npos);

  class HostObjectWithPropertyNames : public HostObject {
    std::vector<PropNameID> getPropertyNames(Runtime& rt) override {
      return PropNameID::names(
          rt, "a_prop", "1", "false", "a_prop", "3", "c_prop");
    }
  };

  Object howpn = Object::createFromHostObject(
      rt, std::make_shared<HostObjectWithPropertyNames>());
  EXPECT_TRUE(
      function(
          "function (o) { return Object.getOwnPropertyNames(o).length == 5 }")
          .call(rt, howpn)
          .getBool());

  auto hasOwnPropertyName = function(
      "function (o, p) {"
      "  return Object.getOwnPropertyNames(o).indexOf(p) >= 0"
      "}");
  EXPECT_TRUE(
      hasOwnPropertyName.call(rt, howpn, String::createFromAscii(rt, "a_prop"))
          .getBool());
  EXPECT_TRUE(
      hasOwnPropertyName.call(rt, howpn, String::createFromAscii(rt, "1"))
          .getBool());
  EXPECT_TRUE(
      hasOwnPropertyName.call(rt, howpn, String::createFromAscii(rt, "false"))
          .getBool());
  EXPECT_TRUE(
      hasOwnPropertyName.call(rt, howpn, String::createFromAscii(rt, "3"))
          .getBool());
  EXPECT_TRUE(
      hasOwnPropertyName.call(rt, howpn, String::createFromAscii(rt, "c_prop"))
          .getBool());
  EXPECT_FALSE(hasOwnPropertyName
                   .call(rt, howpn, String::createFromAscii(rt, "not_existing"))
                   .getBool());
}