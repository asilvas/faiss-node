#include <napi.h>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <faiss/IndexFlat.h>
#include <faiss/index_io.h>
#include <faiss/impl/FaissException.h>
#include <faiss/impl/io.h>
#include <faiss/index_factory.h>
#include <faiss/MetricType.h>
#include <faiss/impl/IDSelector.h>
#include <faiss/IndexHNSW.h>
#include <faiss/IndexIVFFlat.h>
#include <faiss/IVFlib.h>
#include <faiss/IndexIDMap.h>
#include <faiss/invlists/OnDiskInvertedLists.h>

using namespace Napi;
using idx_t = faiss::idx_t;

enum class IndexType
{
  Index = 1,
  IndexFlat = 10,
  IndexFlatL2 = 11,
  IndexFlatIP = 12,
  IndexHNSW = 20,
  IndexIVF = 30,
  IndexIVFFlat = 31,
};

template <class T, typename Y, IndexType IT>
class IndexBase : public Napi::ObjectWrap<T>
{
public:
  IndexBase(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info)
  {
    Napi::Env env = info.Env();

    if constexpr (IT == IndexType::IndexHNSW)
    { // HNSW constructor
      if (info.Length() > 0 && info[0].IsNumber())
      {
        auto d = info[0].As<Napi::Number>().Uint32Value();
        auto m = 32;                                // faiss default
        auto metric = faiss::MetricType::METRIC_L2; // faiss default
        if (info.Length() > 1 && info[1].IsNumber())
        {
          m = info[1].As<Napi::Number>().Uint32Value();
        }
        if (info.Length() > 2 && info[2].IsNumber())
        {
          metric = static_cast<faiss::MetricType>(info[2].As<Napi::Number>().Uint32Value());
        }
        index_ = std::unique_ptr<faiss::IndexHNSW>(new faiss::IndexHNSW(d, m, metric));
      }
    }
    else if constexpr (IT == IndexType::IndexIVFFlat)
    { // IVFFlat constructor
      if (info.Length() > 2 && info[0].IsObject() && info[1].IsNumber() && info[2].IsNumber())
      {
        Napi::Object quantizer = info[0].As<Napi::Object>();
        T *quantizerInstance = Napi::ObjectWrap<T>::Unwrap(quantizer);

        auto d = info[1].As<Napi::Number>().Uint32Value();
        auto nlist = info[2].As<Napi::Number>().Uint32Value();
        auto metric = faiss::MetricType::METRIC_L2; // faiss default
        if (info.Length() > 3 && info[3].IsNumber())
        {
          metric = static_cast<faiss::MetricType>(info[3].As<Napi::Number>().Uint32Value());
        }

        index_ = std::unique_ptr<faiss::IndexIVFFlat>(new faiss::IndexIVFFlat(quantizerInstance->index_.get(), d, nlist, metric));
      }
    }
    else if (info.Length() > 0 && info[0].IsNumber())
    {
      auto n = info[0].As<Napi::Number>().Uint32Value();
      index_ = std::unique_ptr<Y>(new Y(n));
    }
  }

  static Napi::Value read(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a string.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object instance = T::constructor->New({});
    T *index = Napi::ObjectWrap<T>::Unwrap(instance);
    std::string fname = info[0].As<Napi::String>().Utf8Value();

    try
    {
      index->index_ = std::unique_ptr<faiss::Index>(dynamic_cast<faiss::Index *>(faiss::read_index(fname.c_str())));
    }
    catch (const faiss::FaissException &ex)
    {
      Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    }

    return instance;
  }

  static Napi::Value fromBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsBuffer())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a buffer.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object instance = T::constructor->New({});
    T *index = Napi::ObjectWrap<T>::Unwrap(instance);

    auto buffer = Napi::Buffer<uint8_t>::Copy(env, info[0].As<Napi::Buffer<uint8_t>>().Data(), info[0].As<Napi::Buffer<uint8_t>>().Length());
    faiss::VectorIOReader *reader = new faiss::VectorIOReader();
    reader->data = std::vector<uint8_t>(buffer.Data(), buffer.Data() + buffer.Length());

    try
    {
      index->index_ = std::unique_ptr<faiss::Index>(dynamic_cast<faiss::Index *>(faiss::read_index(reader)));
    }
    catch (const faiss::FaissException &ex)
    {
      Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    }

    return instance;
  }

  static Napi::Value fromFactory(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() < 2)
    {
      Napi::Error::New(env, "Expected 2 or 3 arguments, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a number.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "Invalid the second argument type, must be a string.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto metric = faiss::MetricType::METRIC_L2;
    if (info[2].IsNumber())
    {
      metric = static_cast<faiss::MetricType>(info[2].As<Napi::Number>().Uint32Value());
    }

    Napi::Object instance = T::constructor->New({});
    T *index = Napi::ObjectWrap<T>::Unwrap(instance);

    const uint32_t d = info[0].As<Napi::Number>().Uint32Value();
    std::string description = info[1].As<Napi::String>().Utf8Value();

    try
    {
      index->index_ = std::unique_ptr<faiss::Index>(dynamic_cast<faiss::Index *>(faiss::index_factory(d, description.c_str(), metric)));
    }
    catch (const faiss::FaissException &ex)
    {
      Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    }

    return instance;
  }

#ifndef _MSC_VER
  static Napi::Value mergeOnDisk(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 3)
    {
      Napi::Error::New(env, "Expected 3 arguments, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsArray())
    {
      Napi::TypeError::New(env, "Invalid first argument, must be an array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "Invalid second argument, must be a string.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[2].IsString())
    {
      Napi::TypeError::New(env, "Invalid third argument, must be a string.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Array inputArr = info[0].As<Napi::Array>();
    auto inputArrLength = inputArr.Length();
    if (inputArrLength < 2)
    {
      Napi::Error::New(env, "Invalid first argument, array must contain at least 2 entries to merge.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    std::string outIndexFname = info[1].As<Napi::String>().Utf8Value();
    std::string outDataFname = info[2].As<Napi::String>().Utf8Value();

    idx_t ntotal = 0;
    std::vector<const faiss::InvertedLists *> lists;
    faiss::IndexIVF *trainedIndex = nullptr;
    auto trainedIndexOwned = true;
    for (size_t i = 0; i < inputArrLength; i++)
    {
      Napi::Value val = inputArr[i];
      faiss::IndexIVF *mergeIndex = nullptr;
      auto mergeIndexOwned = true;
      if (val.IsObject())
      {
        mergeIndex = faiss::ivflib::extract_index_ivf(Napi::ObjectWrap<T>::Unwrap(val.As<Napi::Object>())->index_.get());
        mergeIndexOwned = false;
      }
      else
      {
        mergeIndex = faiss::ivflib::extract_index_ivf(faiss::read_index(val.As<Napi::String>().Utf8Value().c_str(), faiss::IO_FLAG_MMAP));
      }

      if (i == 0)
      { // first index is trained index
        trainedIndex = mergeIndex;
        trainedIndexOwned = mergeIndexOwned;
        mergeIndexOwned = false;
      }

      mergeIndex->own_invlists = false; // allow IVF to be released without deleting the inverted lists
      lists.push_back(mergeIndex->invlists);
      ntotal += mergeIndex->ntotal;

      if (mergeIndexOwned)
      {
        delete mergeIndex;
      }
    }

    auto il = new faiss::OnDiskInvertedLists(trainedIndex->nlist, trainedIndex->code_size, outDataFname.c_str());
    // this will write the merged index to disk
    il->merge_from(lists.data(), lists.size(), false);

    // replace invertedLists in the output index
    trainedIndex->replace_invlists(il, true);
    trainedIndex->ntotal = ntotal;

    // release all indexes
    if (trainedIndexOwned)
    {
      // write the trained index to disk using the same filename
      faiss::write_index(trainedIndex, outIndexFname.c_str());
      delete trainedIndex;
    }

    for (size_t i = 0; i < lists.size(); i++)
    {
      delete lists[i];
    }

    return env.Undefined();
  }
#endif // _MSC_VER

  Napi::Value getIndexType(const Napi::CallbackInfo &info)
  {
    auto index = index_.get();

    if (dynamic_cast<faiss::IndexFlat *>(index) != nullptr)
    {
      return Napi::Number::New(info.Env(), static_cast<uint32_t>(IndexType::IndexFlat));
    }
    else if (dynamic_cast<faiss::IndexIVF *>(index) != nullptr)
    {
      return Napi::Number::New(info.Env(), static_cast<uint32_t>(IndexType::IndexIVF));
    }
    else if (dynamic_cast<faiss::IndexHNSW *>(index) != nullptr)
    {
      return Napi::Number::New(info.Env(), static_cast<uint32_t>(IndexType::IndexHNSW));
    }

    return Napi::Number::New(info.Env(), static_cast<uint32_t>(IndexType::Index));
  }

  Napi::Value getIsTrained(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    return Napi::Boolean::New(env, index_->is_trained);
  }

  Napi::Value getNTotal(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, index_->ntotal);
  }

  Napi::Value getDimension(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, index_->d);
  }

  Napi::Value getMetricType(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), index_->metric_type);
  }

  Napi::Value getMetricArg(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), index_->metric_arg);
  }

  Napi::Value getCodeSize(const Napi::CallbackInfo &info)
  {
    auto index = dynamic_cast<faiss::IndexFlat *>(index_.get());
    return Napi::Number::New(info.Env(), index->code_size);
  }

  Napi::Value getCodesUInt8(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    auto index = dynamic_cast<faiss::IndexFlat *>(index_.get());
    return Napi::Buffer<uint8_t>::Copy(env, index->codes.data(), index->codes.size());
  }

  Napi::Value getCodesByRange(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    auto index = dynamic_cast<faiss::IndexFlat *>(index_.get());

    size_t start = 0;
    size_t end = index->codes.size();

    if (info.Length() >= 1)
    {
      start = info[0].As<Napi::Number>().Uint32Value();
      if (start >= end)
      {
        Napi::Error::New(env, "Position must be less than end.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    if (info.Length() >= 2)
    {
      end = info[1].As<Napi::Number>().Uint32Value();
      if (end > index->codes.size())
      {
        end = index->codes.size();
      }
    }

    return Napi::Buffer<uint8_t>::Copy(env, &index->codes.data()[start], end - start);
  }

  Napi::Value setCodesByRange(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    auto index = dynamic_cast<faiss::IndexFlat *>(index_.get());

    size_t start = 0;

    if (info.Length() < 1)
    {
      Napi::Error::New(env, "Expected 1 or 2 arguments, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (info.Length() >= 2)
    {
      start = info[1].As<Napi::Number>().Uint32Value();
      if (start >= index->codes.size())
      {
        Napi::Error::New(env, "Position must be less than end.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    auto buffer = info[0].As<Napi::Buffer<uint8_t>>();
    auto length = buffer.Length();
    if (length + start > index->codes.size())
    {
      Napi::Error::New(env, "Buffer size must be less than or equal to the remaining size.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // write buffer to codes
    std::memcpy(&index->codes.data()[start], buffer.Data(), length);

    return env.Undefined();
  }

  Napi::Value getIds(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    auto index = dynamic_cast<faiss::IndexIDMap *>(index_.get());
    auto length = index->id_map.size();
    Napi::Array ids = Napi::Array::New(env, length);
    auto id_map = index->id_map.data();
    for (size_t i = 0; i < length; i++)
    {
      ids[i] = Napi::BigInt::New(env, id_map[i]);
    }

    return ids;
  }

  Napi::Value getNProbe(const Napi::CallbackInfo &info)
  {
    auto index = dynamic_cast<faiss::IndexIVF *>(index_.get());
    return Napi::Number::New(info.Env(), index->nprobe);
  }

  Napi::Value setNProbe(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a Number.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto index = dynamic_cast<faiss::IndexIVF *>(index_.get());
    index->nprobe = info[0].As<Napi::Number>().Int32Value();
    return env.Undefined();
  }

  Napi::Value getEfConstruction(const Napi::CallbackInfo &info)
  {
    auto index = dynamic_cast<faiss::IndexHNSW *>(index_.get());
    return Napi::Number::New(info.Env(), index->hnsw.efConstruction);
  }

  Napi::Value setEfConstruction(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a Number.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto index = dynamic_cast<faiss::IndexHNSW *>(index_.get());
    index->hnsw.efConstruction = info[0].As<Napi::Number>().Int32Value();
    return env.Undefined();
  }

  Napi::Value getEfSearch(const Napi::CallbackInfo &info)
  {
    auto index = dynamic_cast<faiss::IndexHNSW *>(index_.get());
    return Napi::Number::New(info.Env(), index->hnsw.efSearch);
  }

  Napi::Value setEfSearch(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a Number.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto index = dynamic_cast<faiss::IndexHNSW *>(index_.get());
    index->hnsw.efSearch = info[0].As<Napi::Number>().Int32Value();
    return env.Undefined();
  }

  Napi::Value add(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsArray())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be an Array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Array arr = info[0].As<Napi::Array>();
    size_t length = arr.Length();
    auto dv = std::div(length, index_->d);
    if (dv.rem != 0)
    {
      Napi::Error::New(env, "Invalid the given array length.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float *xb = new float[length];
    for (size_t i = 0; i < length; i++)
    {
      Napi::Value val = arr[i];
      xb[i] = val.As<Napi::Number>().FloatValue();
    }

    index_->add(dv.quot, xb);

    delete[] xb;
    return env.Undefined();
  }

  Napi::Value addWithIds(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 2)
    {
      Napi::Error::New(env, "Expected 2 arguments, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsArray())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be an Array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsArray())
    {
      Napi::TypeError::New(env, "Invalid the second argument type, must be an Array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Array arr = info[0].As<Napi::Array>();
    size_t length = arr.Length();
    auto dv = std::div(length, index_->d);
    if (dv.rem != 0)
    {
      Napi::Error::New(env, "Invalid the given array length.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Array labels = info[1].As<Napi::Array>();
    size_t labelCount = labels.Length();
    if (labelCount != dv.quot)
    {
      Napi::Error::New(env, "Labels array length must match the number of vectors.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float *xb = new float[length];
    for (size_t i = 0; i < length; i++)
    {
      Napi::Value val = arr[i];
      xb[i] = val.As<Napi::Number>().FloatValue();
    }

    idx_t *xc = new idx_t[labelCount];
    for (size_t i = 0; i < labelCount; i++)
    {
      Napi::Value val = labels[i];
      if (val.IsNumber())
      {
        xc[i] = val.As<Napi::Number>().Int64Value();
      }
      else if (val.IsBigInt())
      {
        auto lossless = false;
        xc[i] = val.As<Napi::BigInt>().Int64Value(&lossless);
      }
      else
      {
        Napi::Error::New(env, "Expected a Number or BigInt as array item. (at: " + std::to_string(i) + ")")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    index_->add_with_ids(dv.quot, xb, xc);

    delete[] xb;
    delete[] xc;

    return env.Undefined();
  }

  Napi::Value reset(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    index_->reset();

    return env.Undefined();
  }

  Napi::Value dispose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    auto idx = index_.release();
    delete idx;
    index_ = nullptr;

    return env.Undefined();
  }

  Napi::Value train(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsArray())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be an Array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Array arr = info[0].As<Napi::Array>();
    size_t length = arr.Length();
    auto dv = std::div(length, index_->d);
    if (dv.rem != 0)
    {
      Napi::Error::New(env, "Invalid the given array length.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float *xb = new float[length];
    for (size_t i = 0; i < length; i++)
    {
      Napi::Value val = arr[i];
      xb[i] = val.As<Napi::Number>().FloatValue();
    }

    index_->train(dv.quot, xb);

    delete[] xb;
    return env.Undefined();
  }

  Napi::Value search(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    uint32_t k = index_->ntotal;
    if (info.Length() < 1 || !info[0].IsArray())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be an Array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (info.Length() == 2)
    {
      if (!info[1].IsNumber())
      {
        Napi::TypeError::New(env, "Invalid the second argument type, must be a Number.").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      k = info[1].As<Napi::Number>().Uint32Value();
    }

    if (k > index_->ntotal)
    {
      k = index_->ntotal;
    }

    Napi::Array arr = info[0].As<Napi::Array>();
    size_t length = arr.Length();
    auto dv = std::div(length, index_->d);
    if (dv.rem != 0)
    {
      Napi::Error::New(env, "Invalid the given array length.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float *xq = new float[length];
    for (size_t i = 0; i < length; i++)
    {
      Napi::Value val = arr[i];
      xq[i] = val.As<Napi::Number>().FloatValue();
    }

    auto nq = dv.quot;
    idx_t *I = new idx_t[k * nq];
    float *D = new float[k * nq];

    index_->search(nq, xq, k, D, I);

    Napi::Array arr_distances = Napi::Array::New(env, k * nq);
    Napi::Array arr_labels = Napi::Array::New(env, k * nq);
    for (size_t i = 0; i < k * nq; i++)
    {
      idx_t label = I[i];
      float distance = D[i];
      arr_distances[i] = Napi::Number::New(env, distance);
      arr_labels[i] = Napi::BigInt::New(env, label);
    }
    delete[] I;
    delete[] D;

    Napi::Object results = Napi::Object::New(env);
    results.Set("distances", arr_distances);
    results.Set("labels", arr_labels);
    return results;
  }

  Napi::Value reconstruct(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    idx_t key = -1;
    if (info[0].IsNumber())
    {
      key = info[0].As<Napi::Number>().Int64Value();
    }
    else if (info[0].IsBigInt())
    {
      auto lossless = false;
      key = info[0].As<Napi::BigInt>().Int64Value(&lossless);
    }
    else
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a Number or BigInt.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float *inpArr = new float[index_->d];
    Napi::Array outArr = Napi::Array::New(env, index_->d);
    index_->reconstruct(key, inpArr);
    for (size_t i = 0; i < index_->d; i++)
    {
      outArr[i] = Napi::Number::New(env, inpArr[i]);
    }
    delete[] inpArr;

    return outArr;
  }

  Napi::Value reconstructBatch(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsArray())
    {
      Napi::Error::New(env, "Expected 1 array argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Array keysArr = info[0].As<Napi::Array>();
    auto keyCount = keysArr.Length();
    idx_t *keys = new idx_t[keyCount];
    for (size_t i = 0; i < keyCount; i++)
    {
      Napi::Value val = keysArr[i];
      if (val.IsNumber())
      {
        keys[i] = val.As<Napi::Number>().Int64Value();
      }
      else if (val.IsBigInt())
      {
        auto lossless = false;
        keys[i] = val.As<Napi::BigInt>().Int64Value(&lossless);
      }
      else
      {
        Napi::TypeError::New(env, "Invalid the first argument type, must be a Number or BigInt.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    auto dimCount = keyCount * index_->d;
    float *inpArr = new float[dimCount];
    Napi::Array outArr = Napi::Array::New(env, dimCount);
    index_->reconstruct_batch(keyCount, keys, inpArr);
    for (size_t i = 0; i < dimCount; i++)
    {
      outArr[i] = Napi::Number::New(env, inpArr[i]);
    }
    delete[] keys;
    delete[] inpArr;

    return outArr;
  }

  Napi::Value write(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be a string.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    const std::string fname = info[0].As<Napi::String>().Utf8Value();

    faiss::write_index(index_.get(), fname.c_str());

    return env.Undefined();
  }

  Napi::Value mergeFrom(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject())
    {
      Napi::TypeError::New(env, "Invalid argument type, must be an object.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object otherIndex = info[0].As<Napi::Object>();
    T *otherIndexInstance = Napi::ObjectWrap<T>::Unwrap(otherIndex);

    if (otherIndexInstance->index_->d != index_->d)
    {
      Napi::Error::New(env, "The merging index must have the same dimension.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    try
    {
      index_->merge_from(*(otherIndexInstance->index_));
    }
    catch (const faiss::FaissException &ex)
    {
      Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    return env.Undefined();
  }

  Napi::Value removeIds(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
      Napi::Error::New(env, "Expected 1 argument, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsArray())
    {
      Napi::TypeError::New(env, "Invalid the first argument type, must be an Array.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Array arr = info[0].As<Napi::Array>();
    size_t length = arr.Length();

    idx_t *xb = new idx_t[length];
    for (size_t i = 0; i < length; i++)
    {
      Napi::Value val = arr[i];
      if (val.IsNumber())
      {
        xb[i] = val.As<Napi::Number>().Int64Value();
      }
      else if (val.IsBigInt())
      {
        auto lossless = false;
        xb[i] = val.As<Napi::BigInt>().Int64Value(&lossless);
      }
      else
      {
        Napi::Error::New(env, "Expected a Number or BigInt as array item. (at: " + std::to_string(i) + ")")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    size_t num = index_->remove_ids(faiss::IDSelectorArray{length, xb});

    delete[] xb;
    return Napi::Number::New(info.Env(), num);
  }

  Napi::Value toBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 0)
    {
      Napi::Error::New(env, "Expected 0 arguments, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    faiss::VectorIOWriter *writer = new faiss::VectorIOWriter();

    faiss::write_index(index_.get(), writer);

    // return buffer from IOWriter
    return Napi::Buffer<uint8_t>::Copy(env, writer->data.data(), writer->data.size());
  }

  Napi::Value toIDMap2(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() != 0)
    {
      Napi::Error::New(env, "Expected 0 arguments, but got " + std::to_string(info.Length()) + ".")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object instance = T::constructor->New({});
    T *index = Napi::ObjectWrap<T>::Unwrap(instance);

    try
    {
      // wrap the new IDMap'd index around the old and leave it to faiss to throw if index not compatible
      index->index_ = std::unique_ptr<faiss::IndexIDMap2>(new faiss::IndexIDMap2(index_.get()));
    }
    catch (const faiss::FaissException &ex)
    {
      Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    }

    return instance;
  }

protected:
  std::unique_ptr<faiss::Index> index_;
  inline static Napi::FunctionReference *constructor;
};
