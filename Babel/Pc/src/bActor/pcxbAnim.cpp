// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbAnim.cpp
//   Purpose : platform specific animation functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : baEvalAnimTrack3
	Purpose : evaluate animation track 3
	Parameters : destVec, track, time
	Returns : 
	Info : 
*/
void baEvalAnimTrack3(TBVector destVec, TBAnimTrack3 *track, int time)
{
    TBAnimKey3 *keys;
    int low, high;

    keys = track->keys;

    low  = 0;
    high = (int)track->noofKeys - 1;

    // Clamp to last key if time is beyond or at the last key time
    {
        int lastTime = (int)keys[high].time << 10;
        if (time >= lastTime)
        {
            destVec[0] = (float)keys[high].x * track->xQuantScale + track->xQuantBase;
            destVec[1] = (float)keys[high].y * track->yQuantScale + track->yQuantBase;
            destVec[2] = (float)keys[high].z * track->zQuantScale + track->zQuantBase;
            return;
        }
    }

    // Binary search for key interval
    TBAnimKey3 *key = 0;

    while (low <= high)
    {
        int mid = (low + high) >> 1;
        key = &keys[mid];

        int keyTime = (int)key->time << 10;

        if (time == keyTime)
        {
            // Exact key time
            destVec[0] = (float)key->x * track->xQuantScale + track->xQuantBase;
            destVec[1] = (float)key->y * track->yQuantScale + track->yQuantBase;
            destVec[2] = (float)key->z * track->zQuantScale + track->zQuantBase;
            return;
        }

        if (time > keyTime)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    // Now:
    //   high = index of last key with keyTime < time
    //   low  = index of first key with keyTime > time
    // In normal data (time >= first key time) high will be >= 0.
    if (high < 0)
    {
        // Degenerate case: time before first key – clamp to first key
        destVec[0] = (float)keys[0].x * track->xQuantScale + track->xQuantBase;
        destVec[1] = (float)keys[0].y * track->yQuantScale + track->yQuantBase;
        destVec[2] = (float)keys[0].z * track->zQuantScale + track->zQuantBase;
        return;
    }

    int idx0 = high;
    int idx1 = idx0 + 1;

    if (idx1 >= (int)track->noofKeys)
        idx1 = (int)track->noofKeys - 1;

    int t0Full = (int)keys[idx0].time << 10;
    int t1Full = (int)keys[idx1].time << 10;

    int dtNum  = time - t0Full;
    int dtDen  = t1Full - t0Full;

    if (dtDen <= 0)
    {
        // Degenerate interval, fall back to k0
        destVec[0] = (float)keys[idx0].x * track->xQuantScale + track->xQuantBase;
        destVec[1] = (float)keys[idx0].y * track->yQuantScale + track->yQuantBase;
        destVec[2] = (float)keys[idx0].z * track->zQuantScale + track->zQuantBase;
        return;
    }

    float t = (float)dtNum / (float)dtDen;

    float baseX = (float)keys[idx0].x * track->xQuantScale + track->xQuantBase;
    float baseY = (float)keys[idx0].y * track->yQuantScale + track->yQuantBase;
    float baseZ = (float)keys[idx0].z * track->zQuantScale + track->zQuantBase;

    float x1 = (float)keys[idx1].x * track->xQuantScale + track->xQuantBase;
    float y1 = (float)keys[idx1].y * track->yQuantScale + track->yQuantBase;
    float z1 = (float)keys[idx1].z * track->zQuantScale + track->zQuantBase;

    destVec[0] = (x1 - baseX) * t + baseX;
    destVec[1] = (y1 - baseY) * t + baseY;
    destVec[2] = (z1 - baseZ) * t + baseZ;
}

/*	--------------------------------------------------------------------------------
	Function : baEvalAnimTrack4
	Purpose : evaluate animation track 4
	Parameters : destVec, track, time
	Returns : 
	Info : 
*/
void baEvalAnimTrack4(TBVector destVec, TBAnimTrack4 *track, int time)
{
    TBAnimKey4 *keys = track->keys;
    int lastIndex    = track->noofKeys - 1;

    // Clamp to last key if time is at or beyond the last key time
    TBAnimKey4 *lastKey   = &keys[lastIndex];
    int lastKeyTimeScaled = ((int)lastKey->time) << 10;  // time * 0x400

    if (time >= lastKeyTimeScaled)
    {
        destVec[0] = (float)lastKey->x * track->xQuantScale + track->xQuantBase;
        destVec[1] = (float)lastKey->y * track->yQuantScale + track->yQuantBase;
        destVec[2] = (float)lastKey->z * track->zQuantScale + track->zQuantBase;
        destVec[3] = (float)lastKey->w * track->wQuantScale + track->wQuantBase;
        return;
    }

    // Binary search for the key interval
    int lo  = 0;
    int hi  = lastIndex;
    TBAnimKey4 *key = NULL;

    while (lo <= hi)
    {
        int mid = (lo + hi) >> 1;
        key     = &keys[mid];

        int keyTimeScaled = ((int)key->time) << 10;

        if (keyTimeScaled == time)
        {
            // Exact key hit – no interpolation
            destVec[0] = (float)key->x * track->xQuantScale + track->xQuantBase;
            destVec[1] = (float)key->y * track->yQuantScale + track->yQuantBase;
            destVec[2] = (float)key->z * track->zQuantScale + track->zQuantBase;
            destVec[3] = (float)key->w * track->wQuantScale + track->wQuantBase;
            return;
        }

        if (keyTimeScaled < time)
        {
            lo = mid + 1;
        }
        else
        {
            hi = mid - 1;
        }
    }

    // At this point: time is strictly between keys[hi] and keys[hi + 1]
    // (given time is within track range and there are at least two keys).
    int idx0 = hi;
    if (idx0 < 0)
        idx0 = 0;               // safety, should not be hit with valid data

    int idx1 = idx0 + 1;
    if (idx1 > lastIndex)
        idx1 = lastIndex;       // safety, should not be hit because time < lastKeyTimeScaled

    // Dequantize both keys into quaternions
    float q0[4];
    float q1[4];

    q0[0] = (float)keys[idx0].x * track->xQuantScale + track->xQuantBase;
    q0[1] = (float)keys[idx0].y * track->yQuantScale + track->yQuantBase;
    q0[2] = (float)keys[idx0].z * track->zQuantScale + track->zQuantBase;
    q0[3] = (float)keys[idx0].w * track->wQuantScale + track->wQuantBase;

    q1[0] = (float)keys[idx1].x * track->xQuantScale + track->xQuantBase;
    q1[1] = (float)keys[idx1].y * track->yQuantScale + track->yQuantBase;
    q1[2] = (float)keys[idx1].z * track->zQuantScale + track->zQuantBase;
    q1[3] = (float)keys[idx1].w * track->wQuantScale + track->wQuantBase;

    // Fraction between k0 and k1 in the same 0x400 time units
    int t0Scaled = ((int)keys[idx0].time) << 10;
    int t1Scaled = ((int)keys[idx1].time) << 10;

    float frac = (float)(time - t0Scaled) / (float)(t1Scaled - t0Scaled);

    bmQuatSlerp(destVec, q0, q1, frac);
}
