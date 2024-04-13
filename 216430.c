TEST(BasicFlatBufferModel, TestBuildFromNullAllocation) {
  TestErrorReporter reporter;
  std::unique_ptr<Allocation> model_allocation;

  auto model =
      FlatBufferModel::BuildFromAllocation(std::move(model_allocation));
  ASSERT_FALSE(model);
}