TEST_P(JSITest, HostFunctionTest) {
  auto one = std::make_shared<int>(1);
  Function plusOne = Function::createFromHostFunction(
      rt,
      PropNameID::forAscii(rt, "plusOne"),
      2,
      [one, savedRt = &rt](
          Runtime& rt, const Value& thisVal, const Value* args, size_t count) {
        EXPECT_EQ(savedRt, &rt);
        // We don't know if we're in strict mode or not, so it's either global
        // or undefined.
        EXPECT_TRUE(
            Value::strictEquals(rt, thisVal, rt.global()) ||
            thisVal.isUndefined());
        return *one + args[0].getNumber() + args[1].getNumber();
      });

  EXPECT_EQ(plusOne.call(rt, 1, 2).getNumber(), 4);
  EXPECT_TRUE(checkValue(plusOne.call(rt, 3, 5), "9"));
  rt.global().setProperty(rt, "plusOne", plusOne);
  EXPECT_TRUE(eval("plusOne(20, 300) == 321").getBool());

  Function dot = Function::createFromHostFunction(
      rt,
      PropNameID::forAscii(rt, "dot"),
      2,
      [](Runtime& rt, const Value& thisVal, const Value* args, size_t count) {
        EXPECT_TRUE(
            Value::strictEquals(rt, thisVal, rt.global()) ||
            thisVal.isUndefined());
        if (count != 2) {
          throw std::runtime_error("expected 2 args");
        }
        std::string ret = args[0].getString(rt).utf8(rt) + "." +
            args[1].getString(rt).utf8(rt);
        return String::createFromUtf8(
            rt, reinterpret_cast<const uint8_t*>(ret.data()), ret.size());
      });

  rt.global().setProperty(rt, "cons", dot);
  EXPECT_TRUE(eval("cons('left', 'right') == 'left.right'").getBool());
  EXPECT_TRUE(eval("cons.name == 'dot'").getBool());
  EXPECT_TRUE(eval("cons.length == 2").getBool());
  EXPECT_TRUE(eval("cons instanceof Function").getBool());

  EXPECT_TRUE(eval("(function() {"
                   "  try {"
                   "    cons('fail'); return false;"
                   "  } catch (e) {"
                   "    return ((e instanceof Error) &&"
                   "            (e.message == 'Exception in HostFunction: ' +"
                   "                          'expected 2 args'));"
                   "  }})()")
                  .getBool());

  Function coolify = Function::createFromHostFunction(
      rt,
      PropNameID::forAscii(rt, "coolify"),
      0,
      [](Runtime& rt, const Value& thisVal, const Value* args, size_t count) {
        EXPECT_EQ(count, 0);
        std::string ret = thisVal.toString(rt).utf8(rt) + " is cool";
        return String::createFromUtf8(
            rt, reinterpret_cast<const uint8_t*>(ret.data()), ret.size());
      });
  rt.global().setProperty(rt, "coolify", coolify);
  EXPECT_TRUE(eval("coolify.name == 'coolify'").getBool());
  EXPECT_TRUE(eval("coolify.length == 0").getBool());
  EXPECT_TRUE(eval("coolify.bind('R&M')() == 'R&M is cool'").getBool());
  EXPECT_TRUE(eval("(function() {"
                   "  var s = coolify.bind(function(){})();"
                   "  return s.lastIndexOf(' is cool') == (s.length - 8);"
                   "})()")
                  .getBool());

  Function lookAtMe = Function::createFromHostFunction(
      rt,
      PropNameID::forAscii(rt, "lookAtMe"),
      0,
      [](Runtime& rt, const Value& thisVal, const Value* args, size_t count)
          -> Value {
        EXPECT_TRUE(thisVal.isObject());
        EXPECT_EQ(
            thisVal.getObject(rt)
                .getProperty(rt, "name")
                .getString(rt)
                .utf8(rt),
            "mr.meeseeks");
        return Value();
      });
  rt.global().setProperty(rt, "lookAtMe", lookAtMe);
  EXPECT_TRUE(eval("lookAtMe.bind({'name': 'mr.meeseeks'})()").isUndefined());

  struct Callable {
    Callable(std::string s) : str(s) {}

    Value
    operator()(Runtime& rt, const Value&, const Value* args, size_t count) {
      if (count != 1) {
        return Value();
      }
      return String::createFromUtf8(
          rt, args[0].toString(rt).utf8(rt) + " was called with " + str);
    }

    std::string str;
  };

  Function callable = Function::createFromHostFunction(
      rt,
      PropNameID::forAscii(rt, "callable"),
      1,
      Callable("std::function::target"));
  EXPECT_EQ(
      function("function (f) { return f('A cat'); }")
          .call(rt, callable)
          .getString(rt)
          .utf8(rt),
      "A cat was called with std::function::target");
  EXPECT_TRUE(callable.isHostFunction(rt));
  EXPECT_NE(callable.getHostFunction(rt).target<Callable>(), nullptr);

  std::string strval = "strval1";
  auto getter = Object(rt);
  getter.setProperty(
      rt,
      "get",
      Function::createFromHostFunction(
          rt,
          PropNameID::forAscii(rt, "getter"),
          1,
          [&strval](
              Runtime& rt,
              const Value& thisVal,
              const Value* args,
              size_t count) -> Value {
            return String::createFromUtf8(rt, strval);
          }));
  auto obj = Object(rt);
  rt.global()
      .getPropertyAsObject(rt, "Object")
      .getPropertyAsFunction(rt, "defineProperty")
      .call(rt, obj, "prop", getter);
  EXPECT_TRUE(function("function(value) { return value.prop == 'strval1'; }")
                  .call(rt, obj)
                  .getBool());
  strval = "strval2";
  EXPECT_TRUE(function("function(value) { return value.prop == 'strval2'; }")
                  .call(rt, obj)
                  .getBool());
}