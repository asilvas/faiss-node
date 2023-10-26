/** Searh result object. */
export interface SearchResult {
    /** The disances of the nearest negihbors found, size n*k. */
    distances: number[],
    /** The labels of the nearest neighbors found, size n*k. */
    labels: BigInt[]
}

// See faiss/MetricType.h
export enum MetricType {
    METRIC_INNER_PRODUCT = 0, ///< maximum inner product search
    METRIC_L2 = 1,            ///< squared L2 search
    METRIC_L1 = 2,            ///< L1 (aka cityblock)
    METRIC_Linf = 3,          ///< infinity distance
    METRIC_Lp = 4,            ///< L_p distance, p is given by a faiss::Index
    /// metric_arg

    /// some additional metrics defined in scipy.spatial.distance
    METRIC_Canberra = 20,
    METRIC_BrayCurtis = 21,
    METRIC_JensenShannon = 22,
    METRIC_Jaccard = 23,      ///< defined as: sum_i(min(a_i, b_i)) / sum_i(max(a_i, b_i))
    ///< where a_i, b_i > 0
}

/**
 * Index.
 * Index that stores the full vectors and performs exhaustive search.
 * @param {number} d The dimensionality of index.
 */
export class Index {
    constructor(d: number);
    /**
     * @return {numbers} The number of verctors currently indexed.
     */
    get ntotal(): number;
    /**
     * @return {number} The dimensionality of vectors.
     */
    get dims(): number;
    /**
     * returns a boolean that indicates whether training is required.
     * @return {number} Whether training is required.
     */
    get isTrained(): boolean;
    /**
     * @return {MetricType} The metric of the index.
     */
    get metricType(): MetricType;
    /**
     * @return {number} Argument of the metric type.
     */
    get metricArg(): number;
    /** 
     * Add n vectors of dimension d to the index.
     * Vectors are implicitly assigned labels ntotal .. ntotal + n - 1
     * @param {number[]} x Input matrix, size n * d
     */
    add(x: number[]): void;
    /** 
     * Add n vectors of dimension d to the index using the provided labels.
     * @param {number[]} x Input matrix, size n * d
     * @param {(number|BigInt)[]} y Vector identifiers
     */
    addWithIds(x: number[], y: (number|BigInt)[]): void;
    /** 
     * Add n vectors of dimension d to the index with ID's.
     * @param {number[]} x Input matrix, size n * d
     * @param {BigInt[]} ids Vector identifiers
     */
    addWithIds(x: number[], ids: BigInt[]): void;
    /** 
     * Train n vectors of dimension d to the index.
     * Vectors are implicitly assigned labels ntotal .. ntotal + n - 1
     * @param {number[]} x Input matrix, size n * d
     */
    train(x: number[]): void;
    /** 
     * Query n vectors of dimension d to the index.
     * return at most k vectors. If there are not enough results for a
     * query, the result array is padded with -1s.
     *
     * @param {number[]} x Input vectors to search, size n * d.
     * @param {number} k The number of nearest neighbors to search for.
     * @return {SearchResult} Output of the search result.
     */
    search(x: number[], k: number): SearchResult;
    /** 
     * Reconstruct desired vector from index. Will throw if not supported
     * by the index type.
     *
     * @param {number|BigInt} key Key of vector to reconstruct.
     * @return {number[]} Reconstructed vector of length `dims`.
     */
    reconstruct(key: number|BigInt): number[];
    /** 
     * Reconstruct a batch of vectors from index. Will throw if not supported
     * by the index type.
     *
     * @param {(number|BigInt)[]} key Keys of vectors to reconstruct.
     * @return {number[]} Reconstructed vector of length `dims`.
     */
    reconstructBatch(keys: (number|BigInt)[]): number[];
    /** 
     * Write index to a file.
     * @param {string} fname File path to write.
     */
    write(fname: string): void;
    /** 
     * Write index to buffer.
     */
    toBuffer(): Buffer;
    /** 
     * Create an IDMap'd index from source index.
     */
    toIDMap2(): Index;
    /** 
     * Read index from a file.
     * @param {string} fname File path to read.
     * @return {Index} The index read.
     */
    static read(fname: string): Index;
    /** 
     * Read index from buffer.
     * @param {Buffer} src Buffer to create index from.
     * @return {Index} The index read.
     */
    static fromBuffer(src: Buffer): Index;
    /** 
     * Construct an index from factory descriptor.
     * @param {number} dims Buffer to create index from.
     * @param {string} descriptor Factory descriptor.
     * @param {MetricType} metric Metric type (defaults to L2).
     * @return {Index} The index read.
     */
    static fromFactory(dims: number, descriptor: string, metric?: MetricType): Index;
    /**
     * Merge the current index with another Index instance.
     * @param {Index} otherIndex The other Index instance to merge from.
     */
    mergeFrom(otherIndex: Index): void;
    /**
     * Remove IDs from the index.
     * @param {BigInt[]} ids IDs to read.
     * @return {number} number of IDs removed.
     */
    removeIds(ids: BigInt[]): number;
    /**
     * Reset the index, resulting in a ntotal of 0.
     */
    reset(): void;
    /**
     * Free all resources associated with the index. Further calls to the index will throw.
     */
    dispose(): void;
}

/**
 * IndexFlat Abstract Index.
 */
export abstract class IndexFlat extends Index {
    /**
     * Byte size of each encoded vector.
     */
    get codeSize(): number;
    /**
     * Encoded dataset, size ntotal * codeSize.
     */
    get codes(): Buffer;
    /**
     * Vector identifiers, size ntotal.
     */
    get ids(): BigInt[];
    /**
     * Return buffer codes for a given byte range.
     * @param {number} start position to read from (default: 0).
     * @param {number} end position to read to (default: length).
     * @return {Buffer} Buffer containing the requested codes by range.
     */
    getCodesByRange(start?: number, end?: number): Buffer;
    /**
     * Overwrite the codes using a custom buffer.
     * @param {Buffer} buffer data to replace codes with.
     * @param {number} start position to write to (default: 0).
     */
    setCodesByRange(buffer: Buffer, start?: number);
}

/**
 * IndexFlatL2 Index.
 * IndexFlatL2 that stores the full vectors and performs `squared L2` search.
 * @param {number} d The dimensionality of index.
 */
export class IndexFlatL2 extends IndexFlat {
    /** 
     * Read index from a file.
     * @param {string} fname File path to read.
     * @return {IndexFlatL2} The index read.
     */
    static read(fname: string): IndexFlatL2;
    /** 
     * Read index from buffer.
     * @param {Buffer} src Buffer to create index from.
     * @return {IndexFlatL2} The index read.
     */
    static fromBuffer(src: Buffer): IndexFlatL2;
    /**
     * Merge the current index with another IndexFlatL2 instance.
     * @param {IndexFlatL2} otherIndex The other IndexFlatL2 instance to merge from.
     */
    mergeFrom(otherIndex: IndexFlatL2): void;
}

/**
 * IndexFlatIP Index.
 * Index that stores the full vectors and performs `maximum inner product` search.
 * @param {number} d The dimensionality of index.
 */
export class IndexFlatIP extends IndexFlat {
    /** 
     * Read index from a file.
     * @param {string} fname File path to read.
     * @return {IndexFlatIP} The index read.
     */
    static read(fname: string): IndexFlatIP;
    /** 
     * Read index from buffer.
     * @param {Buffer} src Buffer to create index from.
     * @return {IndexFlatIP} The index read.
     */
    static fromBuffer(src: Buffer): IndexFlatIP;
    /**
     * Merge the current index with another IndexFlatIP instance.
     * @param {IndexFlatIP} otherIndex The other IndexFlatIP instance to merge from.
     */
    mergeFrom(otherIndex: IndexFlatIP): void;
}

/**
 * IndexHNSW Index.
 * The Hierarchical Navigable Small World indexing method is based on a graph built on the indexed vectors.
 * @param {number} d The dimensionality of index.
 * @param {number} m The number of neighbors used in the graph (defaults to 32).
 * @param {number} metric Metric type (defaults to L2).
 */
export class IndexHNSW extends Index {
    IndexHNSW(d?: number, m?: number, metric?: MetricType);
    /** 
     * Read index from a file.
     * @param {string} fname File path to read.
     * @return {IndexHNSW} The index read.
     */
    static read(fname: string): IndexHNSW;
    /** 
     * Read index from buffer.
     * @param {Buffer} src Buffer to create index from.
     * @return {IndexHNSW} The index read.
     */
    static fromBuffer(src: Buffer): IndexHNSW;
    /**
     * Merge the current index with another IndexHNSW instance.
     * @param {IndexHNSW} otherIndex The other IndexHNSW instance to merge from.
     */
    mergeFrom(otherIndex: IndexHNSW): void;
    /**
     * The depth of exploration at add time.
     */
    get efConstruction(): number;
    /**
     * The depth of exploration at add time.
     * @param {number} value The value to set.
     */
    set efConstruction(value: number);
    /**
     * The depth of exploration of the search.
     */
    get efSearch(): number;
    /**
     * The depth of exploration of the search.
     * @param {number} value The value to set.
     */
    set efSearch(value: number);
}

/**
 * IndexIVFFlat Index.
 * Inverted file with stored vectors.
 * @param {Index} quantizer.
 * @param {number} d The dimensionality of index.
 * @param {number} nlist Number of clusters.
 * @param {number} metric Metric type (defaults to L2).
 */
export class IndexIVFFlat extends Index {
    IndexIVFFlat(quantizer: Index, d: number, nlist: number, metric?: MetricType);
    /** 
     * Read index from a file.
     * @param {string} fname File path to read.
     * @return {IndexIVFFlat} The index read.
     */
    static read(fname: string): IndexIVFFlat;
    /** 
     * Read index from buffer.
     * @param {Buffer} src Buffer to create index from.
     * @return {IndexIVFFlat} The index read.
     */
    static fromBuffer(src: Buffer): IndexIVFFlat;
    /** 
     * Merge trained & untrained IVF indexes on disk.
     * @param {(string|IndexIVFFlat)[]} inputIdxOrPaths IVF indexes (or paths) to merge, with the first being trained.
     * @param {string} outputIndexPath Path of final merged index file.
     * @param {string} outputDataPath Path of final merged data file.
     */
    static mergeOnDisk(mergePaths: string[], outputIndexPath: string, outputDataPath: string): void;
    /**
     * Merge the current index with another IndexIVFFlat instance.
     * @param {IndexIVFFlat} otherIndex The other IndexIVFFlat instance to merge from.
     */
    mergeFrom(otherIndex: IndexIVFFlat): void;
    /**
     * Cells to search.
     */
    get nprobe(): number;
    /**
     * Cells to search.
     * @param {number} value The value to set.
     */
    set nprobe(value: number);
    /**
     * Vector identifiers, size ntotal.
     */
    get ids(): BigInt[];
}