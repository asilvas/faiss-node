const faiss = require('bindings')('faiss-napi');

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

const allIndexes = [faiss.Index, faiss.IndexFlatL2, faiss.IndexFlatIP, faiss.IndexHNSW];

// all indexes
wireupGetterSetters('ntotal', allIndexes, 'getNTotal');
wireupGetterSetters('dims', allIndexes, 'getDimension');
wireupGetterSetters('isTrained', allIndexes, 'getIsTrained');
wireupGetterSetters('metricType', allIndexes, 'getMetricType');
wireupGetterSetters('metricArg', allIndexes, 'getMetricArg');
wireupGetterSetters('ids', allIndexes, 'getIds');

// Flat
wireupGetterSetters('codeSize', [faiss.IndexFlatL2, faiss.IndexFlatIP], 'getCodeSize');
wireupGetterSetters('codes', [faiss.IndexFlatL2, faiss.IndexFlatIP], 'getCodesUInt8');

// HNSW
wireupGetterSetters('efConstruction', [faiss.IndexHNSW], 'getEfConstruction', 'setEfConstruction');
wireupGetterSetters('efSearch', [faiss.IndexHNSW], 'getEfSearch', 'setEfSearch');

// IVF
wireupGetterSetters('nprobe', [faiss.IndexIVFFlat], 'getNProbe', 'setNProbe');

module.exports = faiss;