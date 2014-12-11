// -*- C++ -*-
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2010 Amir Saffari, amir@ymer.org
 * Copyright (C) 2010 Amir Saffari, 
 *                    Institute for Computer Graphics and Vision, 
 *                    Graz University of Technology, Austria
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <cstdlib>
#include <time.h>
#include <ctime>
#include <fstream>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Random numbers
inline unsigned int getDevRandom() {
    ifstream devFile("/dev/urandom", ios::binary);
    unsigned int outInt = 0;
    char tempChar[sizeof(outInt)];

    devFile.read(tempChar, sizeof(outInt));
    outInt = atoi(tempChar);

    devFile.close();

    return outInt;
}

inline double randDouble() {
    static bool seedFlag = false;

    if (!seedFlag) {
        struct timeval TV;
        gettimeofday(&TV, NULL);
        srand(TV.tv_sec * TV.tv_usec + getpid() + getDevRandom());
        seedFlag = true;
    }
    return ((double) rand()) / RAND_MAX;
}

inline double randDouble(const double& minRange, const double& maxRange) {
    return (minRange + (maxRange - minRange) * randDouble());
}

inline int randInteger(const int& minRange, const int& maxRange) {
    return (int) (minRange + (maxRange - minRange) * randDouble());
}

//! Poisson sampling
inline int poisson(double A) {
    int k = 0;
    int maxK = 10;
    while (1) {
        double U_k = randDouble();
        A *= U_k;
        if (k > maxK || A < exp(-1.0)) {
            break;
        }
        k++;
    }
    return k;
}

inline void randPerm(const int& inNum, vector<int>& outVect) {
    outVect.resize(inNum);
    int randIndex, tempIndex;
    for (int nFeat = 0; nFeat < inNum; nFeat++) {
        outVect[nFeat] = nFeat;
    }
    for (register int nFeat = 0; nFeat < inNum; nFeat++) {
        randIndex = (int) floor(((double) inNum - nFeat) * randDouble()) + nFeat;
        if (randIndex == inNum) {
            randIndex--;
        }
        tempIndex = outVect[nFeat];
        outVect[nFeat] = outVect[randIndex];
        outVect[randIndex] = tempIndex;
    }
}

inline void randPerm(const int& inNum, const int& inPart, vector<int>& outVect) {
    outVect.resize(inNum);
    int randIndex, tempIndex;
    for (int nFeat = 0; nFeat < inNum; nFeat++) {
        outVect[nFeat] = nFeat;
    }
    for (register int nFeat = 0; nFeat < inPart; nFeat++) {
        randIndex = (int) floor(((double) inNum - nFeat) * randDouble()) + nFeat;
        if (randIndex == inNum) {
            randIndex--;
        }
        tempIndex = outVect[nFeat];
        outVect[nFeat] = outVect[randIndex];
        outVect[randIndex] = tempIndex;
    }

    outVect.erase(outVect.begin() + inPart, outVect.end());
}

#endif /* UTILITIES_H_ */
