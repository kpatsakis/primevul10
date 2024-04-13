TEST(BasicFlatBufferModel, TestNullDestination) {
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/empty_model.bin");
  ASSERT_TRUE(model);
  // Test that building with null destination fails.
  ASSERT_NE(InterpreterBuilder(*model, TrivialResolver())(nullptr), kTfLiteOk);
}