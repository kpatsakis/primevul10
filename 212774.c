TEST_P(JSITest, MultiDecoratorTest) {
  struct Inc {
    void before() {
      ++count;
    }

    // Test optional after method.

    int count = 0;
  };

  struct Nest {
    void before() {
      ++nest;
    }

    void after() {
      --nest;
    }

    int nest = 0;
  };

  class MultiRuntime final
      : public WithRuntimeDecorator<std::tuple<Inc, Nest>> {
   public:
    explicit MultiRuntime(std::unique_ptr<Runtime> rt)
        : WithRuntimeDecorator<std::tuple<Inc, Nest>>(*rt, tuple_),
          rt_(std::move(rt)) {}

    int count() {
      return std::get<0>(tuple_).count;
    }
    int nest() {
      return std::get<1>(tuple_).nest;
    }

   private:
    std::unique_ptr<Runtime> rt_;
    std::tuple<Inc, Nest> tuple_;
  };

  MultiRuntime mrt(factory());

  Function expectNestOne = Function::createFromHostFunction(
      mrt,
      PropNameID::forAscii(mrt, "expectNestOne"),
      0,
      [](Runtime& rt, const Value& thisVal, const Value* args, size_t count) {
        MultiRuntime* funcmrt = dynamic_cast<MultiRuntime*>(&rt);
        EXPECT_NE(funcmrt, nullptr);
        EXPECT_EQ(funcmrt->count(), 3);
        EXPECT_EQ(funcmrt->nest(), 1);
        return Value::undefined();
      });

  expectNestOne.call(mrt);

  EXPECT_EQ(mrt.count(), 3);
  EXPECT_EQ(mrt.nest(), 0);
}