const { IndexFlatL2, IndexIVFFlat } = require('..');

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
});
