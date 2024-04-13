TEST(BasicFlatBufferModel, TestEmptyModels) {
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/empty_model.bin");
  ASSERT_TRUE(model);
  // Now try to build it into a model.
  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(InterpreterBuilder(*model, TrivialResolver())(&interpreter),
            kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
}