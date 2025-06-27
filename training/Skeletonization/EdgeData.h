#pragma once

class EdgeData
{
public:
    EdgeData()
    {
        mAddedToMeshLoop = false;
    }
    
    EdgeData(int start, int end):
    mStart(start),
    mEnd(end)
    {
        mAddedToMeshLoop = false;
    }

    EdgeData& operator=(const EdgeData& other)
    {
        if (this != &other) 
        {
            mStart = other.mStart;
            mEnd = other.mEnd;
        }
        return *this;
    }

    bool operator==(const EdgeData& other) const
    {
        return mStart == other.mStart && mEnd == other.mEnd;
    }

    bool operator<(const EdgeData& other) const 
    {
        if (mStart != other.mStart)
            return mStart < other.mStart;
        return mEnd < other.mEnd;
    }

    // mStart will be less than mEnd
    int mStart = -1;
    int mEnd = -1;

    bool mAddedToMeshLoop = false;
};