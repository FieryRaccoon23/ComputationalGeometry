#pragma once

class PointData
{
public:
    PointData(){}
    PointData(int id, float x, float y):
    mId(id),
    mX(x),
    mY(y)
    {
    }

    PointData& operator=(const PointData& other)
    {
        if (this != &other) 
        {
            mId = other.mId;
            mX = other.mX;
            mY = other.mY;
        }
        return *this;
    }

    bool operator==(const PointData& other) const
    {
        return mId == other.mId;
    }

    int mId = -1;
    float mX = 0.0f;
    float mY = 0.0f;
};