TEST_P(JSITest, DecoratorTest) {
  struct Count {
    // init here is just to show that a With type does not need to be
    // default constructible.
    explicit Count(int init) : count(init) {}

    // Test optional before method.

    void after() {
      ++count;
    }

    int count;
  };

  static constexpr int kInit = 17;

  class CountRuntime final : public WithRuntimeDecorator<Count> {
   public:
    explicit CountRuntime(std::unique_ptr<Runtime> rt)
        : WithRuntimeDecorator<Count>(*rt, count_),
          rt_(std::move(rt)),
          count_(kInit) {}

    int count() {
      return count_.count;
    }

   private:
    std::unique_ptr<Runtime> rt_;
    Count count_;
  };

  CountRuntime crt(factory());

  crt.description();
  EXPECT_EQ(crt.count(), kInit + 1);

  crt.global().setProperty(crt, "o", Object(crt));
  EXPECT_EQ(crt.count(), kInit + 6);
}