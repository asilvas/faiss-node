# faiss-napi
[![NPM Version](https://img.shields.io/npm/v/faiss-napi?logo=npm)](https://www.npmjs.com/package/faiss-napi)
[![Node Version](https://img.shields.io/node/v/faiss-napi)](https://github.com/asilvas/faiss-node)
[![Unit Test](https://github.com/ewfian/faiss-node/actions/workflows/unit_test.yml/badge.svg)](https://github.com/asilvas/faiss-node/actions/workflows/unit_test.yml)
[![License](https://img.shields.io/github/license/asilvas/faiss-node)](https://github.com/asilvas/faiss-node)
[![Documentation](https://img.shields.io/badge/api-reference-blue.svg)](https://asilvas.github.io/faiss-node/)


faiss-napi provides Node/Bun/Deno NAPI bindings for [faiss](https://github.com/facebookresearch/faiss)

_**This package is a fork of the original `faiss-node` with a focus on advanced features & performance.**_


## Installation

```sh
$ npm install faiss-napi
```

## Documentation

* [faiss-napi API Documentation](https://asilvas.github.io/faiss-node/)

## Usage

```javascript
const { IndexFlatL2, Index, IndexFlatIP, IndexHNSW, MetricType } = require('faiss-napi');

const dimension = 2;
const index = new IndexFlatL2(dimension);

console.log(index.dims); // 2
console.log(index.isTrained); // true
console.log(index.ntotal); // 0

// inserting data into index.
index.add([1, 0]);
index.add([1, 2]);
index.add([1, 3]);
index.add([1, 1]);

console.log(index.ntotal); // 4

const k = 4;
const results = index.search([1, 0], k);
console.log(results.labels); // [ 0n, 3n, 1n, 2n ]
console.log(results.distances); // [ 0, 1, 4, 9 ]

// Save index
const fname = 'faiss.index';
index.write(fname);

// Load saved index
const index_loaded = IndexFlatL2.read(fname);
console.log(index_loaded.dims); //2
console.log(index_loaded.ntotal); //4
const results1 = index_loaded.search([1, 1], 4);
console.log(results1.labels); // [ 3n, 0n, 1n, 2n ]
console.log(results1.distances); // [ 0, 1, 1, 4 ]

// Merge index
const newIndex = new IndexFlatL2(dimension);
newIndex.mergeFrom(index);
console.log(newIndex.ntotal); // 4

// Remove items
console.log(newIndex.search([1, 2], 1)); // { distances: [ 0 ], labels: [ 1n ] }
const removedCount = newIndex.removeIds([0]);
console.log(removedCount); // 1
console.log(newIndex.ntotal); // 3
console.log(newIndex.search([1, 2], 1)); // { distances: [ 0 ], labels: [ 0n ] }

// IndexFlatIP
const ipIndex = new IndexFlatIP(2);
ipIndex.add([1, 0]);

// Serialize an index
const index_buf = newIndex.toBuffer();
const deserializedIndex = Index.fromBuffer(index_buf);
console.log(deserializedIndex.ntotal); // 3

// Factory index
const hnswIndex = Index.fromFactory(2, 'HNSW32,Flat', MetricType.METRIC_INNER_PRODUCT);
// same as:
// const hnswIndex = new IndexHNSW(2, 32, MetricType.METRIC_INNER_PRODUCT)
const x = [1, 0, 0, 1];
hnswIndex.train(x);
hnswIndex.add(x);

// IDMap'd index
const idIndex = new IndexFlatL2(2).toIDMap2();
const vectors = [[1, 0], [0, 1]];
idIndex.addWithIds(vectors.flat(), [100n, 200n]);
// reconstruct vectors
expect(idIndex.reconstruct(idIndex.ids[0])).toEqual(vectors[0]);
expect(idIndex.reconstructBatch(idIndex.ids)).toEqual(vectors.flat());

// IVF
const ivf = new IndexIVFFlat(new IndexFlatL2(2), 2, 2);
const x = Array.from({ length: 400 }, () => Math.random());
const y = Array.from({ length: 200 }, (_, i) => i);
trained.train(x.slice(0, 200));
trained.addWithIds(x.slice(0, 200), y.slice(0, 100));
trained.write('trained.ivf');
trained.addWithIds(x.slice(200), y.slice(100));
trained.write('untrained.ivf');
IndexIVFFlat.mergeOnDisk('trained.ivf', ['untrained.ivf'], 'merged.ivf');
```

## License

MIT