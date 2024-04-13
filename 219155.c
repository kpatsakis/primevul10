void Interpreter::saveGenerator(
    Runtime *runtime,
    PinnedHermesValue *frameRegs,
    const Inst *resumeIP) {
  auto *innerFn = vmcast<GeneratorInnerFunction>(FRAME.getCalleeClosure());
  innerFn->saveStack(runtime);
  innerFn->setNextIP(resumeIP);
  innerFn->setState(GeneratorInnerFunction::State::SuspendedYield);
}