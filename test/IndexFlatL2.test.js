const { IndexFlatL2 } = require('..');

describe('IndexFlatL2', () => {
    describe('#read', () => {
        it('throws an error if file does not existed', () => {
            const fname = 'not_existed_file'
            expect(() => { IndexFlatL2.read(fname) }).toThrow(new RegExp(`^Error.*could not open ${fname} for reading: No such file or directory$`));
        });

        it('read saved file.', () => {
            const dimension = 2;
            const index = new IndexFlatL2(dimension);
            index.add([1, 0]);

            const fname = '_tmp.read.index';
            index.write(fname);

            const index_loaded = IndexFlatL2.read(fname);
            expect(index_loaded.dims).toBe(2);
            expect(index_loaded.ntotal).toBe(1);
        })
    });

    describe('#ntotal', () => {
        const index = new IndexFlatL2(1);

        it('returns 0 if the index is just initialized', () => {
            expect(index.ntotal).toBe(0);
        });

        it('returns total count', () => {
            index.add([1]);
            expect(index.ntotal).toBe(1);
            index.add([1, 2, 3]);
            expect(index.ntotal).toBe(4);
        });
    });

    describe('#dims', () => {
        const index = new IndexFlatL2(128);

        it('returns dimension', () => {
            expect(index.dims).toBe(128);
        });
    });

    describe('#isTrained', () => {
        const index = new IndexFlatL2(1);

        it('returns true fixed', () => {
            expect(index.isTrained).toBe(true);
        });
    });

    describe('#add', () => {
        const index = new IndexFlatL2(2);

        it('throws an error if the count of given param is not 1', () => {
            expect(() => { index.add() }).toThrow('Expected 1 argument, but got 0.');
            expect(() => { index.add([], 1) }).toThrow('Expected 1 argument, but got 2.');
        });

        it('throws an error if given a non-Array object', () => {
            expect(() => { index.add('[1, 2, 3]') }).toThrow('Invalid the first argument type, must be an Array.');
        });

        it('throws an error if the length of given array is not adhere to the dimension of the index', () => {
            expect(() => { index.add([1, 2, 3]) }).toThrow('Invalid the given array length.');
            expect(() => { index.add([1, 2, 3, 4, 5]) }).toThrow('Invalid the given array length.');
            expect(() => { index.add([1]) }).toThrow('Invalid the given array length.');
        });
    });

    describe('#search', () => {
        const index = new IndexFlatL2(2);

        beforeAll(() => {
            index.add([1, 0]);
            index.add([1, 2]);
            index.add([1, 3]);
            index.add([1, 1]);
        });

        it('throws an error if the count of given param is not 1 or 2', () => {
            expect(() => { index.search() }).toThrow('Invalid the first argument type, must be an Array.');
        });

        it('throws an error if given a non-Array object to first argument', () => {
            expect(() => { index.search('[1, 2, 3]', 2) }).toThrow('Invalid the first argument type, must be an Array.');
        });

        it('throws an error if given a non-Number object to second argument', () => {
            expect(() => { index.search([1, 2, 3], '2') }).toThrow('Invalid the second argument type, must be a Number.');
        });

        it('returns ntotal results if 2nd argument not provided', () => {
            expect(index.search([1, 1])).toMatchObject({ distances: [0, 1, 1, 4], labels: [3n, 0n, 1n, 2n] });
        });

        it('never returns more than ntotal results', () => {
            expect(index.search([1, 1], 6)).toMatchObject({ distances: [0, 1, 1, 4], labels: [3n, 0n, 1n, 2n] });
        });

        it('throws an error if the length of given array is not adhere to the dimension of the index', () => {
            expect(() => { index.search([1, 2, 3], 2) }).toThrow('Invalid the given array length.');
            expect(() => { index.search([1, 2, 3, 4, 5], 2) }).toThrow('Invalid the given array length.');
            expect(() => { index.search([1], 2) }).toThrow('Invalid the given array length.');
        });

        it('returns search results', () => {
            expect(index.search([1, 0], 1)).toMatchObject({ distances: [0], labels: [0n] });
            expect(index.search([1, 0], 4)).toMatchObject({ distances: [0, 1, 4, 9], labels: [0n, 3n, 1n, 2n] });
            expect(index.search([1, 1], 4)).toMatchObject({ distances: [0, 1, 1, 4], labels: [3n, 0n, 1n, 2n] });
        });
    });

    describe("#merge", () => {
        const index1 = new IndexFlatL2(2);
        beforeAll(() => {
            index1.add([1, 0]);
            index1.add([1, 2]);
            index1.add([1, 3]);
            index1.add([1, 1]);
        });

        const index2 = new IndexFlatL2(2);
        beforeAll(() => {
            index2.mergeFrom(index1);
        });

        it("throws an error if the number of arguments is not 1", () => {
            expect(() => { index2.mergeFrom() }).toThrow('Expected 1 argument, but got 0.');
            expect(() => { index2.mergeFrom(index1, 2) }).toThrow('Expected 1 argument, but got 2.');
        });

        it("throws an error if argument is not an object", () => {
            expect(() => { index2.mergeFrom(1) }).toThrow('Invalid argument type, must be an object.');
            expect(() => { index2.mergeFrom("string") }).toThrow('Invalid argument type, must be an object.');
        });

        it("throws an error if merging index has different dimensions", () => {
            const index3 = new IndexFlatL2(3);
            expect(() => { index2.mergeFrom(index3) }).toThrow('The merging index must have the same dimension.');
        });

        it("returns search results on merged index", () => {
            expect(index2.search([1, 0], 1)).toMatchObject({
                distances: [0],
                labels: [0n],
            });
            expect(index2.search([1, 0], 4)).toMatchObject({
                distances: [0, 1, 4, 9],
                labels: [0n, 3n, 1n, 2n],
            });
            expect(index2.search([1, 1], 4)).toMatchObject({
                distances: [0, 1, 1, 4],
                labels: [3n, 0n, 1n, 2n],
            });
        });
    });

    describe("#removeIds", () => {
        let index;
        beforeEach(() => {
            index = new IndexFlatL2(2);
            index.add([1, 0]);
            index.add([1, 1]);
            index.add([1, 2]);
            index.add([1, 3]);
        });

        it('throws an error if the count of given param is not 1', () => {
            expect(() => { index.removeIds() }).toThrow('Expected 1 argument, but got 0.');
            expect(() => { index.removeIds([], 1) }).toThrow('Expected 1 argument, but got 2.');
        });

        it('throws an error if given a non-Array object', () => {
            expect(() => { index.removeIds('[1, 2, 3]') }).toThrow('Invalid the first argument type, must be an Array.');
        });

        it("returns number of IDs removed", () => {
            expect(index.removeIds([])).toBe(0);
            expect(index.removeIds([0])).toBe(1);
            expect(index.removeIds([0, 1])).toBe(2);
            expect(index.removeIds([2])).toBe(0);
            expect(index.removeIds([0, 1, 2])).toBe(1);
        });

        it("correctly removed", () => {
            expect(index.search([1, 1], 1)).toMatchObject({ distances: [0], labels: [1n] });
            expect(index.removeIds([0])).toBe(1);
            expect(index.search([1, 1], 1)).toMatchObject({ distances: [0], labels: [0n] });
        });

        it("correctly removed multiple elements", () => {
            expect(index.search([1, 3], 1)).toMatchObject({ distances: [0], labels: [3n] });
            expect(index.removeIds([0, 1])).toBe(2);
            expect(index.search([1, 3], 1)).toMatchObject({ distances: [0], labels: [1n] });
        });

        it("correctly removed partal elements", () => {
            expect(index.search([1, 3], 1)).toMatchObject({ distances: [0], labels: [3n] });
            expect(index.removeIds([0, 1, 2, 4, 5])).toBe(3);
            expect(index.search([1, 3], 1)).toMatchObject({ distances: [0], labels: [0n] });
        });
    });

    describe('#codes', () => {
        it("returns codeSize", () => {
            const index = new IndexFlatL2(2);
            expect(index.codeSize).toBe(8);
        });

        it("returns codes", () => {
            const index = new IndexFlatL2(2);
            const arr = [1, 1, 255, 255];
            index.add(arr.slice(0, 2));
            index.add(arr.slice(2, 4));
            expect(index.codes).toStrictEqual(Buffer.from(Float32Array.from(arr).buffer));
            index.add([99, 99]);
            expect(index.codes).toStrictEqual(Buffer.from(Float32Array.from(arr.concat([99, 99])).buffer));
        });

        it("getCodesByRange defaults is same as codes", () => {
            const index = new IndexFlatL2(2);
            const arr = [1, 1, 255, 255];
            index.add(arr.slice(0, 2));
            index.add(arr.slice(2, 4));
            expect(index.getCodesByRange()).toStrictEqual(Buffer.from(Float32Array.from(arr).buffer));
            index.add([99, 99]);
            expect(index.getCodesByRange()).toStrictEqual(Buffer.from(Float32Array.from(arr.concat([99, 99])).buffer));
        });

        it("getCodesByRange to only return 1st vector codes", () => {
            const index = new IndexFlatL2(2);
            const arr = [1, 1, 255, 255];
            index.add(arr.slice(0, 2));
            index.add(arr.slice(2, 4));
            expect(index.getCodesByRange(0, 2 * 4)).toStrictEqual(Buffer.from(Float32Array.from(arr.slice(0, 2)).buffer));
        });

        it("getCodesByRange to only return 2nd vector codes", () => {
            const index = new IndexFlatL2(2);
            const arr = [1, 1, 255, 255];
            index.add(arr.slice(0, 2));
            index.add(arr.slice(2, 4));
            expect(index.getCodesByRange(2 * 4)).toStrictEqual(Buffer.from(Float32Array.from(arr.slice(2, 4)).buffer));
        });

        it("setCodesByRange to replace codes on 1st vector only", () => {
            const index = new IndexFlatL2(2);
            const arr = [1, 1, 255, 255];
            index.add(arr.slice(0, 2));
            index.add(arr.slice(2, 4));
            expect(index.getCodesByRange(0, 2 * 4)).toStrictEqual(Buffer.from(Float32Array.from(arr.slice(0, 2)).buffer));
            index.setCodesByRange(Buffer.from(Float32Array.from(arr.slice(2, 4)).buffer), 0);
            expect(index.getCodesByRange(0, 2 * 4)).toStrictEqual(Buffer.from(Float32Array.from(arr.slice(2, 4)).buffer));
        });

        it("setCodesByRange to replace codes on 2nd vector only", () => {
            const index = new IndexFlatL2(2);
            const arr = [1, 1, 255, 255];
            index.add(arr.slice(0, 2));
            index.add(arr.slice(2, 4));
            expect(index.getCodesByRange(2 * 4)).toStrictEqual(Buffer.from(Float32Array.from(arr.slice(2, 4)).buffer));
            index.setCodesByRange(Buffer.from(Float32Array.from(arr.slice(0, 2)).buffer), 2 * 4);
            expect(index.getCodesByRange(2 * 4)).toStrictEqual(Buffer.from(Float32Array.from(arr.slice(0, 2)).buffer));
        });
    });
});