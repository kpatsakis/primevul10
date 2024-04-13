  Object createObject(std::shared_ptr<HostObject> ho) {
    class DHO2 : public DecoratedHostObject {
     public:
      using DecoratedHostObject::DecoratedHostObject;

      Value get(Runtime& rt, const PropNameID& name) override {
        numGets++;
        return DecoratedHostObject::get(rt, name);
      }
    };
    return Object::createFromHostObject(
        plain(), std::make_shared<DHO2>(*this, ho));
  }