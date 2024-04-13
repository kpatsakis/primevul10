TEST_P(JSITest, MultilevelDecoratedHostObject) {
  // This test will be run for various test instantiations, so initialize these
  // counters.
  RD1::numGets = 0;
  RD2::numGets = 0;

  RD1 rd1(rt);
  RD2 rd2(rd1);
  // We expect the "get" operation of ho to be called with rd2.
  auto ho = std::make_shared<HO>(&rd2);
  auto obj = Object::createFromHostObject(rd2, ho);
  Value v = obj.getProperty(rd2, "p");
  EXPECT_TRUE(v.isNumber());
  EXPECT_EQ(17.0, v.asNumber());
  auto ho2 = obj.getHostObject(rd2);
  EXPECT_EQ(ho, ho2);
  EXPECT_EQ(1, RD1::numGets);
  EXPECT_EQ(1, RD2::numGets);
}