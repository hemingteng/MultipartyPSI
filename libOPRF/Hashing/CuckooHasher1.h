#pragma once
#include "Common/Defines.h"
#include "Common/Log.h"
#include "Common/BitVector.h"
#include "Common/ArrayView.h"
#include "Common/MatrixView.h"
//#include <mutex>
#include <atomic>

//#define THREAD_SAFE_CUCKOO

namespace osuCrypto
{
    struct CuckooParam1
    {
        u64 mStashSize;
        double mBinScaler;
        u64 mNumHashes, mSenderBinSize;
    };



    class CuckooHasher1
    {
    public:
        CuckooHasher1();
        ~CuckooHasher1();

        struct Bin
        {
            Bin() :mVal(-1) {}
            Bin(u64 idx, u64 hashIdx) : mVal(idx | (hashIdx << 56)) {}

            bool isEmpty() const;
            u64 idx() const;
            u64 hashIdx() const;

            void swap(u64& idx, u64& hashIdx);
#ifdef THREAD_SAFE_CUCKOO
            Bin(const Bin& b) : mVal(b.mVal.load(std::memory_order_relaxed)) {}
            Bin(Bin&& b) : mVal(b.mVal.load(std::memory_order_relaxed)) {}
            std::atomic<u64> mVal;
#else
            Bin(const Bin& b) : mVal(b.mVal) {}
            Bin(Bin&& b) : mVal(b.mVal) {}
            u64 mVal;
#endif
        };
        struct Workspace
        {
            Workspace(u64 n)
                : curAddrs(n)
                , curHashIdxs(n)
                , oldVals(n)
                //, findAddr(n)
                , findVal(n)
            {}

            std::vector<u64>
                curAddrs,// (inputIdxs.size(), 0),
                curHashIdxs,// (inputIdxs.size(), 0),
                oldVals;// (inputIdxs.size());

            std::vector<std::array<u64, 2>> /*findAddr,*/ findVal;
        };



        u64 mTotalTries;

        bool operator==(const CuckooHasher1& cmp)const;
        bool operator!=(const CuckooHasher1& cmp)const;

        //std::mutex mStashx;

        CuckooParam1 mParams;
		block mHashSeed;
		u64 mBinCount, mMaxBinSize, mRepSize, mInputBitSize, mN;
        void print() const;
		void init(u64 n, block hashSeed, u64 statSecParam, bool multiThreaded);
        void insert(u64 IdxItem, ArrayView<u64> hashes);
        void insertHelper(u64 IdxItem, u64 hashIdx, u64 numTries);

        void insertBatch(ArrayView<u64> itemIdxs, MatrixView<u64> hashs, Workspace& workspace);

        u64 find(ArrayView<u64> hashes);
        u64 findBatch(MatrixView<u64> hashes, 
            ArrayView<u64> idxs,
            Workspace& wordkspace);

   // private:

        std::vector<u64> mHashes;
        MatrixView<u64> mHashesView;

        std::vector<Bin> mBins;
        std::vector<Bin> mStash;

        //std::vector<Bin> mBins;
        //std::vector<Bin> mStash;


        //void insertItems(std::array<std::vector<block>,4>& hashs);
    };

}