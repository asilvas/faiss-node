const { Index, MetricType } = require('..');

describe('Index', () => {
  describe('#fromFactory', () => {
    it('Flat', () => {
      const index = Index.fromFactory(2, 'Flat');
      const x = [1, 0, 0, 1];
      index.add(x);

      expect(index.ntotal).toBe(2);
    });

    it('Flat /w IP', () => {
      const index = Index.fromFactory(2, 'Flat', MetricType.METRIC_INNER_PRODUCT);
      const x = [1, 0, 0, 1];
      index.add(x);

      expect(index.ntotal).toBe(2);
    });
  });

  describe('#train', () => {
    it('HNSW training', () => {
      const index = Index.fromFactory(2, 'HNSW,Flat');
      const x = [1, 0, 0, 1];
      index.train(x);
      index.add(x);

      expect(index.ntotal).toBe(2);
    });
  });

  describe('#toBuffer', () => {
    it('new index is same size as old', () => {
      const index = Index.fromFactory(2, 'Flat');
      const x = [1, 0, 0, 1];
      index.add(x);

      const buf = index.toBuffer();
      const newIndex = Index.fromBuffer(buf);

      expect(index.ntotal).toBe(newIndex.ntotal);
    });
  });

  describe('#metricType', () => {
    it('metric adheres to default', () => {
      const index = Index.fromFactory(2, 'Flat');
      expect(index.metricType).toBe(MetricType.METRIC_L2);
      expect(index.metricArg).toBe(0);
    });

    it('metric adheres to initialized value', () => {
      const index = Index.fromFactory(2, 'Flat', MetricType.METRIC_INNER_PRODUCT);
      expect(index.metricType).toBe(MetricType.METRIC_INNER_PRODUCT);
    });
  });

  describe('#toIDMap2', () => {
    it('new index preserves ID\'s', () => {
      const index = Index.fromFactory(2, 'Flat').toIDMap2();
      const x = [1, 0, 0, 1];
      const labels = [100n, 200n];
      index.addWithIds(x, labels);
      const results = index.search([1, 0], 2);
      expect(results.labels).toEqual(labels);
    });

    it('supports BigInt labels', () => {
      const index = Index.fromFactory(2, 'Flat').toIDMap2();
      const x = [1, 0, 0, 1];
      const labels = [100n, 200n];
      index.addWithIds(x, labels);
      const results = index.search([1, 0], 2);
      expect(results.labels).toEqual(labels);
    });

    it('can fetch IDs', () => {
      const index = Index.fromFactory(2, 'Flat').toIDMap2();
      const x = [1, 2, 3, 4];
      const labels = [100n, 200n];
      index.addWithIds(x, labels);
      expect(index.ids).toEqual(labels);
    });
  });

  describe('#reset', () => {
    let index;

    beforeEach(() => {
      index = Index.fromFactory(2, 'Flat');
      index.add([1, 0, 0, 1]);
    });

    it('reset the index', () => {
      expect(index.ntotal).toBe(2);
      index.reset();
      expect(index.ntotal).toBe(0);
    });

    it('reset the index and add new elements', () => {
      expect(index.ntotal).toBe(2);
      index.reset();
      expect(index.ntotal).toBe(0);

      index.add([1, 0]);
      index.add([1, 2]);
      expect(index.ntotal).toBe(2);
    });
  });

  describe('#dispose', () => {
    let index;

    beforeEach(() => {
      index = Index.fromFactory(2, 'Flat');
      index.add([1, 0, 0, 1]);
    });

    it('disposing an index does not throw', () => {
      index.dispose();
    });
  });
});
