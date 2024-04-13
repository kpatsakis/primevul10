TEST(TestAddDelegateOwnership, AddDelegateDoesNotTakeOwnership) {
  class TestDelegate : public TfLiteDelegate {
   public:
    TestDelegate(bool* destroyed, bool* prepared)
        : TfLiteDelegate(TfLiteDelegateCreate()),
          destroyed_(destroyed),
          prepared_(prepared) {
      flags = kTfLiteDelegateFlagsNone;
      Prepare = [](TfLiteContext*, TfLiteDelegate* delegate) -> TfLiteStatus {
        *(static_cast<TestDelegate*>(delegate)->prepared_) = true;
        return kTfLiteOk;
      };
    }
    ~TestDelegate() { *destroyed_ = true; }

   private:
    bool* destroyed_;
    bool* prepared_;
  };

  // Construct a delegate with flags for indicating preparation/destruction.
  bool destroyed = false;
  bool prepared = false;
  {
    std::unique_ptr<TestDelegate> delegate(
        new TestDelegate(&destroyed, &prepared));
    {
      // Load a model.
      auto model = FlatBufferModel::BuildFromFile(
          "tensorflow/lite/testdata/empty_model.bin");
      ASSERT_TRUE(model);
      // Now try to build it into an interpreter.
      std::unique_ptr<Interpreter> interpreter;
      InterpreterBuilder builder(*model, TrivialResolver());
      builder.AddDelegate(delegate.get());  // Does not transfer ownership.
      // Loop to check we can construct multiple interpreters from one builder.
      for (int i = 0; i < 3; i++) {
        prepared = false;
        ASSERT_EQ(builder(&interpreter), kTfLiteOk);
        ASSERT_NE(interpreter, nullptr);

        // The delegate should be prepared as normal, and should be preserved.
        EXPECT_TRUE(prepared);
        EXPECT_FALSE(destroyed);

        // Interpreter interaction should not impact the delegate's validity.
        interpreter->AllocateTensors();
        interpreter->Invoke();
        EXPECT_FALSE(destroyed);
      }
    }
    EXPECT_NE(delegate, nullptr);
    EXPECT_FALSE(destroyed);
  }
  // Only after the delegate itself goes out of scope should the delegate be
  // destroyed.
  EXPECT_TRUE(destroyed);
}