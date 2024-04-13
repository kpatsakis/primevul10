TEST(BasicFlatBufferModel, TestBuildFromAllocation) {
  TestErrorReporter reporter;
  std::unique_ptr<Allocation> model_allocation(new FileCopyAllocation(
      "tensorflow/lite/testdata/test_model.bin", &reporter));
  ASSERT_TRUE(model_allocation->valid());

  auto model =
      FlatBufferModel::BuildFromAllocation(std::move(model_allocation));
  ASSERT_TRUE(model);

  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(
      InterpreterBuilder(*model, TrivialResolver(&dummy_reg))(&interpreter),
      kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
}