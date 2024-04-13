TEST_P(JSITest, HostObjectWithValueMembers) {
  class Bag : public HostObject {
   public:
    Bag() = default;

    const Value& operator[](const std::string& name) const {
      auto iter = data_.find(name);
      if (iter == data_.end()) {
        return undef_;
      }
      return iter->second;
    }

   protected:
    Value get(Runtime& rt, const PropNameID& name) override {
      return Value(rt, (*this)[name.utf8(rt)]);
    }

    void set(Runtime& rt, const PropNameID& name, const Value& val) override {
      data_.emplace(name.utf8(rt), Value(rt, val));
    }

    Value undef_;
    std::map<std::string, Value> data_;
  };

  auto sharedBag = std::make_shared<Bag>();
  auto& bag = *sharedBag;
  Object jsbag = Object::createFromHostObject(rt, std::move(sharedBag));
  auto set = function(
      "function (o) {"
      "  o.foo = 'bar';"
      "  o.count = 37;"
      "  o.nul = null;"
      "  o.iscool = true;"
      "  o.obj = { 'foo': 'bar' };"
      "}");
  set.call(rt, jsbag);
  auto checkFoo = function("function (o) { return o.foo === 'bar'; }");
  auto checkCount = function("function (o) { return o.count === 37; }");
  auto checkNul = function("function (o) { return o.nul === null; }");
  auto checkIsCool = function("function (o) { return o.iscool === true; }");
  auto checkObj = function(
      "function (o) {"
      "  return (typeof o.obj) === 'object' && o.obj.foo === 'bar';"
      "}");
  // Check this looks good from js
  EXPECT_TRUE(checkFoo.call(rt, jsbag).getBool());
  EXPECT_TRUE(checkCount.call(rt, jsbag).getBool());
  EXPECT_TRUE(checkNul.call(rt, jsbag).getBool());
  EXPECT_TRUE(checkIsCool.call(rt, jsbag).getBool());
  EXPECT_TRUE(checkObj.call(rt, jsbag).getBool());

  // Check this looks good from c++
  EXPECT_EQ(bag["foo"].getString(rt).utf8(rt), "bar");
  EXPECT_EQ(bag["count"].getNumber(), 37);
  EXPECT_TRUE(bag["nul"].isNull());
  EXPECT_TRUE(bag["iscool"].getBool());
  EXPECT_EQ(
      bag["obj"].getObject(rt).getProperty(rt, "foo").getString(rt).utf8(rt),
      "bar");
}