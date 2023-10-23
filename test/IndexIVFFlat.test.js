const { IndexFlatL2, IndexIVFFlat } = require('..');
const { readdirSync, unlinkSync } = require('fs');

afterEach(() => {
  readdirSync('.').filter((f) => f.startsWith('_tmp')).forEach((f) => unlinkSync(f));
});

describe('IndexIVFFlat', () => {
  describe('#constructor', () => {
    it('3 args will result in index', () => {
      const quantizer = new IndexFlatL2(2);
      const index = new IndexIVFFlat(quantizer, 2, 2);
      expect(index.nprobe).toBe(1);
    });
  });

  describe('#nprobe', () => {
    it('Default value', () => {
      const quantizer = new IndexFlatL2(2);
      const index = new IndexIVFFlat(quantizer, 2, 2);
      expect(index.nprobe).toBe(1);
    });

    it('Set new value', () => {
      const quantizer = new IndexFlatL2(2);
      const index = new IndexIVFFlat(quantizer, 2, 2);
      expect(index.nprobe).toBe(1);
      index.nprobe = 8;
      expect(index.nprobe).toBe(8);
    });
  });

  describe('#mergeOnDisk', () => {
    it('Can merge indexes from disk', () => {
      const quantizer = new IndexFlatL2(2);
      const trained = new IndexIVFFlat(quantizer, 2, 2);
      const x = Array.from({ length: 400 }, () => Math.random());
      const y = Array.from({ length: 200 }, (_, i) => i);
      trained.train(x.slice(0, 200));
      trained.addWithIds(x.slice(0, 200), y.slice(0, 100));
      trained.write('_tmp.test.trained.ivf');
      trained.addWithIds(x.slice(200), y.slice(100));
      trained.write('_tmp.test.untrained.ivf');
      IndexIVFFlat.mergeOnDisk(['_tmp.test.trained.ivf', '_tmp.test.untrained.ivf'], '_tmp.test.merged.ivf');
    });

    it('Can merge indexes from memory', () => {
      const quantizer = new IndexFlatL2(2);
      let trained = new IndexIVFFlat(quantizer, 2, 2);
      const x = Array.from({ length: 400 }, () => Math.random());
      const y = Array.from({ length: 200 }, (_, i) => i);
      trained.train(x.slice(0, 200));
      trained.addWithIds(x.slice(0, 200), y.slice(0, 100));
      trained.write('_tmp.test.trained.ivf');
      trained.addWithIds(x.slice(200), y.slice(100));
      trained.write('_tmp.test.untrained.ivf');
      trained = IndexIVFFlat.read('_tmp.test.trained.ivf'); // restore checkpoint
      const untrained = IndexIVFFlat.read('_tmp.test.untrained.ivf');
      IndexIVFFlat.mergeOnDisk([trained, untrained], '_tmp.test.merged.ivf');
    });
  });
});
