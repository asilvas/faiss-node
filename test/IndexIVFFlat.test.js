const { Index, IndexFlatL2, IndexIVFFlat } = require('..');
const { readdirSync, unlinkSync } = require('fs');
const os = require('os');

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
    it('Can merge indexes on disk', () => {
      if (os.platform() === 'win32') return; // windows doesn't support merging on disk
      if (process.env.MKL_SKIP) return;

      const trained = Index.fromFactory(2, 'IVF2,Flat');
      const x = Array.from({ length: 400 }, () => Math.random());
      const y = Array.from({ length: 200 }, (_, i) => i);
      trained.train(x.slice(0, 200));
      trained.write('_tmp.trained.ivf');
      const untrained = IndexIVFFlat.read('_tmp.trained.ivf');
      untrained.addWithIds(x, y);
      untrained.write('_tmp.block.ivf');
      IndexIVFFlat.mergeOnDisk(['_tmp.trained.ivf', '_tmp.block.ivf'], '_tmp.merged.ivf', '_tmp.merged.ivfdata');
      expect(() => IndexIVFFlat.read('_tmp.merged.ivf')).not.toThrow(); // is valid index check
      expect(IndexIVFFlat.read('_tmp.merged.ivf').ntotal).toBe(200);
    });

    it('Can merge multiple blocks on disk', () => {
      if (os.platform() === 'win32') return; // windows doesn't support merging on disk
      if (process.env.MKL_SKIP) return;

      let trained = Index.fromFactory(2, 'IVF2,Flat');
      const x = Array.from({ length: 600 }, () => Math.random());
      const y = Array.from({ length: 300 }, (_, i) => i);
      trained.train(x.slice(0, 200));
      trained.write('_tmp.trained.ivf');
      let untrained = IndexIVFFlat.read('_tmp.trained.ivf');
      untrained.addWithIds(x.slice(0, 400), y.slice(0, 200));
      untrained.write('_tmp.block.ivf');
      IndexIVFFlat.mergeOnDisk(['_tmp.trained.ivf', '_tmp.block.ivf'], '_tmp.merged.ivf', '_tmp.merged.ivfdata');
      expect(() => IndexIVFFlat.read('_tmp.merged.ivf')).not.toThrow();
      expect(IndexIVFFlat.read('_tmp.merged.ivf').ntotal).toBe(200);
      untrained = IndexIVFFlat.read('_tmp.trained.ivf');
      untrained.addWithIds(x.slice(400), y.slice(200));
      untrained.write('_tmp.block2.ivf');
      IndexIVFFlat.mergeOnDisk(['_tmp.trained.ivf', '_tmp.block.ivf', '_tmp.block2.ivf'], '_tmp.merged.ivf', '_tmp.merged.ivfdata');
      expect(() => IndexIVFFlat.read('_tmp.merged.ivf')).not.toThrow();
      expect(IndexIVFFlat.read('_tmp.merged.ivf').ntotal).toBe(300);
    });

    it('Can merge indexes in memory', () => {
      if (os.platform() === 'win32') return; // windows doesn't support merging on disk
      if (process.env.MKL_SKIP) return;

      const trained = Index.fromFactory(2, 'IVF2,Flat');
      const x = Array.from({ length: 400 }, () => Math.random());
      const y = Array.from({ length: 200 }, (_, i) => i);
      trained.train(x);
      trained.write('_tmp.trained.ivf');
      expect(trained.ntotal).toBe(0);
      let untrained = IndexIVFFlat.read('_tmp.trained.ivf');
      untrained.addWithIds(x, y);
      untrained.write('_tmp.block.ivf');
      expect(untrained.ntotal).toBe(200);
      IndexIVFFlat.mergeOnDisk([trained, untrained], '_tmp.merged.ivf', '_tmp.merged.ivfdata');
      expect(trained.ntotal).toBe(200);
    });
  });
});
