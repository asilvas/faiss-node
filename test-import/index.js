const assert = require('assert');
const { IndexFlatL2 } = require('faiss-napi');

const dimension = 4096;
const index = new IndexFlatL2(dimension);
assert.equal(index.dims, dimension);

console.log(process.version);
console.log(process.platform);
console.log(process.arch);
console.log(index.dims);
