/** AUTO-GENERATED, DO NOT EDIT. SEE scripts/prebuild.js & indexes.json **/
#include <napi.h>
#include "faiss.cc"

class Index : public IndexBase<Index, faiss::IndexFlatL2, IndexType::Index>
{
public:
  using IndexBase::IndexBase;

  static constexpr const char *CLASS_NAME = "Index";

  static Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    // clang-format off
    auto func = DefineClass(env, CLASS_NAME, {
      InstanceMethod("getDimension", &Index::getDimension),
      InstanceMethod("getNTotal", &Index::getNTotal),
      InstanceMethod("getIsTrained", &Index::getIsTrained),
      InstanceMethod("getMetricType", &Index::getMetricType),
      InstanceMethod("getMetricArg", &Index::getMetricArg),
      InstanceMethod("add", &Index::add),
      InstanceMethod("addWithIds", &Index::addWithIds),
      InstanceMethod("train", &Index::train),
      InstanceMethod("search", &Index::search),
      InstanceMethod("reset", &Index::reset),
      InstanceMethod("dispose", &Index::dispose),
      InstanceMethod("write", &Index::write),
      InstanceMethod("mergeFrom", &Index::mergeFrom),
      InstanceMethod("removeIds", &Index::removeIds),
      InstanceMethod("toBuffer", &Index::toBuffer),
      InstanceMethod("toIDMap2", &Index::toIDMap2),
      StaticMethod("fromBuffer", &Index::fromBuffer),
      StaticMethod("read", &Index::read),
      StaticMethod("fromFactory", &Index::fromFactory),
    });
    // clang-format on

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set(CLASS_NAME, func);
    return exports;
  }
};

class IndexFlatL2 : public IndexBase<IndexFlatL2, faiss::IndexFlatL2, IndexType::IndexFlatL2>
{
public:
  using IndexBase::IndexBase;

  static constexpr const char *CLASS_NAME = "IndexFlatL2";

  static Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    // clang-format off
    auto func = DefineClass(env, CLASS_NAME, {
      InstanceMethod("getDimension", &IndexFlatL2::getDimension),
      InstanceMethod("getNTotal", &IndexFlatL2::getNTotal),
      InstanceMethod("getIsTrained", &IndexFlatL2::getIsTrained),
      InstanceMethod("getMetricType", &IndexFlatL2::getMetricType),
      InstanceMethod("getMetricArg", &IndexFlatL2::getMetricArg),
      InstanceMethod("add", &IndexFlatL2::add),
      InstanceMethod("addWithIds", &IndexFlatL2::addWithIds),
      InstanceMethod("train", &IndexFlatL2::train),
      InstanceMethod("search", &IndexFlatL2::search),
      InstanceMethod("reset", &IndexFlatL2::reset),
      InstanceMethod("dispose", &IndexFlatL2::dispose),
      InstanceMethod("write", &IndexFlatL2::write),
      InstanceMethod("mergeFrom", &IndexFlatL2::mergeFrom),
      InstanceMethod("removeIds", &IndexFlatL2::removeIds),
      InstanceMethod("toBuffer", &IndexFlatL2::toBuffer),
      InstanceMethod("toIDMap2", &IndexFlatL2::toIDMap2),
      InstanceMethod("getCodesByRange", &IndexFlatL2::getCodesByRange),
      InstanceMethod("setCodesByRange", &IndexFlatL2::setCodesByRange),
      InstanceMethod("getCodesUInt8", &IndexFlatL2::getCodesUInt8),
      InstanceMethod("getCodeSize", &IndexFlatL2::getCodeSize),
      StaticMethod("fromBuffer", &IndexFlatL2::fromBuffer),
      StaticMethod("read", &IndexFlatL2::read),
    });
    // clang-format on

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set(CLASS_NAME, func);
    return exports;
  }
};

class IndexFlatIP : public IndexBase<IndexFlatIP, faiss::IndexFlatIP, IndexType::IndexFlatIP>
{
public:
  using IndexBase::IndexBase;

  static constexpr const char *CLASS_NAME = "IndexFlatIP";

  static Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    // clang-format off
    auto func = DefineClass(env, CLASS_NAME, {
      InstanceMethod("getDimension", &IndexFlatIP::getDimension),
      InstanceMethod("getNTotal", &IndexFlatIP::getNTotal),
      InstanceMethod("getIsTrained", &IndexFlatIP::getIsTrained),
      InstanceMethod("getMetricType", &IndexFlatIP::getMetricType),
      InstanceMethod("getMetricArg", &IndexFlatIP::getMetricArg),
      InstanceMethod("add", &IndexFlatIP::add),
      InstanceMethod("addWithIds", &IndexFlatIP::addWithIds),
      InstanceMethod("train", &IndexFlatIP::train),
      InstanceMethod("search", &IndexFlatIP::search),
      InstanceMethod("reset", &IndexFlatIP::reset),
      InstanceMethod("dispose", &IndexFlatIP::dispose),
      InstanceMethod("write", &IndexFlatIP::write),
      InstanceMethod("mergeFrom", &IndexFlatIP::mergeFrom),
      InstanceMethod("removeIds", &IndexFlatIP::removeIds),
      InstanceMethod("toBuffer", &IndexFlatIP::toBuffer),
      InstanceMethod("toIDMap2", &IndexFlatIP::toIDMap2),
      InstanceMethod("getCodesByRange", &IndexFlatIP::getCodesByRange),
      InstanceMethod("setCodesByRange", &IndexFlatIP::setCodesByRange),
      InstanceMethod("getCodesUInt8", &IndexFlatIP::getCodesUInt8),
      InstanceMethod("getCodeSize", &IndexFlatIP::getCodeSize),
      StaticMethod("fromBuffer", &IndexFlatIP::fromBuffer),
      StaticMethod("read", &IndexFlatIP::read),
    });
    // clang-format on

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set(CLASS_NAME, func);
    return exports;
  }
};

class IndexHNSW : public IndexBase<IndexHNSW, faiss::IndexHNSW, IndexType::IndexHNSW>
{
public:
  using IndexBase::IndexBase;

  static constexpr const char *CLASS_NAME = "IndexHNSW";

  static Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    // clang-format off
    auto func = DefineClass(env, CLASS_NAME, {
      InstanceMethod("getDimension", &IndexHNSW::getDimension),
      InstanceMethod("getNTotal", &IndexHNSW::getNTotal),
      InstanceMethod("getIsTrained", &IndexHNSW::getIsTrained),
      InstanceMethod("getMetricType", &IndexHNSW::getMetricType),
      InstanceMethod("getMetricArg", &IndexHNSW::getMetricArg),
      InstanceMethod("add", &IndexHNSW::add),
      InstanceMethod("addWithIds", &IndexHNSW::addWithIds),
      InstanceMethod("train", &IndexHNSW::train),
      InstanceMethod("search", &IndexHNSW::search),
      InstanceMethod("reset", &IndexHNSW::reset),
      InstanceMethod("dispose", &IndexHNSW::dispose),
      InstanceMethod("write", &IndexHNSW::write),
      InstanceMethod("mergeFrom", &IndexHNSW::mergeFrom),
      InstanceMethod("removeIds", &IndexHNSW::removeIds),
      InstanceMethod("toBuffer", &IndexHNSW::toBuffer),
      InstanceMethod("toIDMap2", &IndexHNSW::toIDMap2),
      InstanceMethod("getEfConstruction", &IndexHNSW::getEfConstruction),
      InstanceMethod("setEfConstruction", &IndexHNSW::setEfConstruction),
      InstanceMethod("getEfSearch", &IndexHNSW::getEfSearch),
      InstanceMethod("setEfSearch", &IndexHNSW::setEfSearch),
      StaticMethod("fromBuffer", &IndexHNSW::fromBuffer),
      StaticMethod("read", &IndexHNSW::read),
    });
    // clang-format on

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set(CLASS_NAME, func);
    return exports;
  }
};

class IndexIVFFlat : public IndexBase<IndexIVFFlat, faiss::IndexIVFFlat, IndexType::IndexIVFFlat>
{
public:
  using IndexBase::IndexBase;

  static constexpr const char *CLASS_NAME = "IndexIVFFlat";

  static Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    // clang-format off
    auto func = DefineClass(env, CLASS_NAME, {
      InstanceMethod("getDimension", &IndexIVFFlat::getDimension),
      InstanceMethod("getNTotal", &IndexIVFFlat::getNTotal),
      InstanceMethod("getIsTrained", &IndexIVFFlat::getIsTrained),
      InstanceMethod("getMetricType", &IndexIVFFlat::getMetricType),
      InstanceMethod("getMetricArg", &IndexIVFFlat::getMetricArg),
      InstanceMethod("add", &IndexIVFFlat::add),
      InstanceMethod("addWithIds", &IndexIVFFlat::addWithIds),
      InstanceMethod("train", &IndexIVFFlat::train),
      InstanceMethod("search", &IndexIVFFlat::search),
      InstanceMethod("reset", &IndexIVFFlat::reset),
      InstanceMethod("dispose", &IndexIVFFlat::dispose),
      InstanceMethod("write", &IndexIVFFlat::write),
      InstanceMethod("mergeFrom", &IndexIVFFlat::mergeFrom),
      InstanceMethod("removeIds", &IndexIVFFlat::removeIds),
      InstanceMethod("toBuffer", &IndexIVFFlat::toBuffer),
      InstanceMethod("toIDMap2", &IndexIVFFlat::toIDMap2),
      InstanceMethod("getNProbe", &IndexIVFFlat::getNProbe),
      InstanceMethod("setNProbe", &IndexIVFFlat::setNProbe),
      StaticMethod("fromBuffer", &IndexIVFFlat::fromBuffer),
      StaticMethod("read", &IndexIVFFlat::read),
    });
    // clang-format on

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set(CLASS_NAME, func);
    return exports;
  }
};

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  Index::Init(env, exports);
  IndexFlatL2::Init(env, exports);
  IndexFlatIP::Init(env, exports);
  IndexHNSW::Init(env, exports);
  IndexIVFFlat::Init(env, exports);

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);
