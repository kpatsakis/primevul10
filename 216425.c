TEST(BasicFlatBufferModel, TestBuildFromInvalidAllocation) {
  TestErrorReporter reporter;
  std::unique_ptr<Allocation> model_allocation(
      new MemoryAllocation(nullptr, 0, nullptr));

  auto model =
      FlatBufferModel::BuildFromAllocation(std::move(model_allocation));
  ASSERT_FALSE(model);
}