const path = require('path');

const faiss = require('bindings')({
  bindings: 'faiss-napi',
  module_root: path.resolve(__dirname, '..'),
});

faiss.MetricType = void 0;
var MetricType;
(function (MetricType) {
  MetricType[MetricType["METRIC_INNER_PRODUCT"] = 0] = "METRIC_INNER_PRODUCT";
  MetricType[MetricType["METRIC_L2"] = 1] = "METRIC_L2";
  MetricType[MetricType["METRIC_L1"] = 2] = "METRIC_L1";
  MetricType[MetricType["METRIC_Linf"] = 3] = "METRIC_Linf";
  MetricType[MetricType["METRIC_Lp"] = 4] = "METRIC_Lp";
  MetricType[MetricType["METRIC_Canberra"] = 20] = "METRIC_Canberra";
  MetricType[MetricType["METRIC_BrayCurtis"] = 21] = "METRIC_BrayCurtis";
  MetricType[MetricType["METRIC_JensenShannon"] = 22] = "METRIC_JensenShannon";
  MetricType[MetricType["METRIC_Jaccard"] = 23] = "METRIC_Jaccard";
})(MetricType || (faiss.MetricType = MetricType = {}));

faiss.IndexType = void 0;
var IndexType;
(function (IndexType) {
  IndexType[IndexType["Index"] = 1] = "Index";
  IndexType[IndexType["IndexFlat"] = 10] = "IndexFlat";
  IndexType[IndexType["IndexFlatL2"] = 11] = "IndexFlatL2";
  IndexType[IndexType["IndexFlatIP"] = 12] = "IndexFlatIP";
  IndexType[IndexType["IndexHNSW"] = 20] = "IndexHNSW";
  IndexType[IndexType["IndexIVF"] = 30] = "IndexIVF";
  IndexType[IndexType["IndexIVFFlat"] = 31] = "IndexIVFFlat";
})(IndexType || (faiss.IndexType = IndexType = {}));

function wireupGetterSetters(propName, indexes, getter, setter) {
  for (let Index of indexes) {
    if (!(propName in Index.prototype)) { // prevents redefinition in jest
      const args = {};
      if (getter) {
        args['get'] = function () {
          return this[getter]();
        }
      }
      if (setter) {
        args['set'] = function (v) {
          this[setter](v);
        }
      }
      Object.defineProperty(Index.prototype, propName, args);
    }
  }
}

const allIndexes = [faiss.Index, faiss.IndexFlatL2, faiss.IndexFlatIP, faiss.IndexHNSW, faiss.IndexIVFFlat];

// all indexes
wireupGetterSetters('ntotal', allIndexes, 'getNTotal');
wireupGetterSetters('dims', allIndexes, 'getDimension');
wireupGetterSetters('isTrained', allIndexes, 'getIsTrained');
wireupGetterSetters('metricType', allIndexes, 'getMetricType');
wireupGetterSetters('metricArg', allIndexes, 'getMetricArg');
wireupGetterSetters('ids', allIndexes, 'getIds');
wireupGetterSetters('indexType', allIndexes, 'getIndexType');

// Flat
wireupGetterSetters('codeSize', [faiss.IndexFlatL2, faiss.IndexFlatIP], 'getCodeSize');
wireupGetterSetters('codes', [faiss.IndexFlatL2, faiss.IndexFlatIP], 'getCodesUInt8');

// HNSW
wireupGetterSetters('efConstruction', [faiss.IndexHNSW], 'getEfConstruction', 'setEfConstruction');
wireupGetterSetters('efSearch', [faiss.IndexHNSW], 'getEfSearch', 'setEfSearch');

// IVF
wireupGetterSetters('nprobe', [faiss.IndexIVFFlat], 'getNProbe', 'setNProbe');

module.exports = faiss;