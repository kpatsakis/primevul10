  Value get(Runtime& rt, const PropNameID& name) override {
    EXPECT_EQ(expectedRT_, &rt);
    return Value(17.0);
  }