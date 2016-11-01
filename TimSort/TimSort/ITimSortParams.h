#pragma once

enum EWhatMerge {
    WM_NoMerge,
    WM_MergeXY,
    WM_MergeYZ
};

class ITimSortParams {
public:
    virtual size_t minRun(const size_t& n) const = 0;

    virtual bool needMerge(const size_t& lenX, const size_t& lenY) const = 0;

    virtual EWhatMerge whatMerge(const size_t& lenX, const size_t& lenY, const size_t& lenZ) const = 0;

    virtual size_t getGallop() const = 0;
};

class DefaultParamsRealisation : public ITimSortParams {
public:
    size_t minRun(const size_t& n) const {
        size_t r = 0, n1 = n;
        while (n1 >= 64) {
            r |= n & 1;
            n1 >>= 1;
        }
        return n1 + r;
    }

    bool needMerge(const size_t& lenX, const size_t& lenY) const {
        return lenX <= lenY;
    }

    EWhatMerge whatMerge(const size_t& lenX, const size_t& lenY, const size_t& lenZ) const {
        if (lenY > lenZ && lenX > lenY + lenZ)
            return WM_NoMerge;
        if (lenX <= lenZ)
            return WM_MergeXY;
        return WM_MergeYZ;
    }

    size_t getGallop() const {
        return 7;
    }
};