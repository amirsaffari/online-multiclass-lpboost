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

#include "online_mclpboost.h"

OnlineMCLPBoost::OnlineMCLPBoost(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, 
                                 const VectorXd& minFeatRange, const VectorXd& maxFeatRange) :
    Booster(hp, numClasses, numFeatures, minFeatRange, maxFeatRange), m_nuD(hp.nuD), m_nuP(hp.nuP) {
    m_name = "OnlineMCLPBoost";
}

void OnlineMCLPBoost::update(Sample& sample) {
    // Update the cache
    if ((int) m_cache.size() == m_hp->cacheSize) {
        m_cache.erase(m_cache.begin());
    }
    m_cache.push_back(Cache(sample, m_hp->numBases, *m_numClasses));

    // Evaluate the samples in cache
    for (int nCache = 0; nCache < (int) m_cache.size(); nCache++) {
        m_cache[nCache].cacheSample.w = m_hp->C;

        // Evaluate this sample and find yPrime
        Result yPrimeResults(*m_numClasses);
        eval(m_cache[nCache].cacheSample, yPrimeResults);
        m_cache[nCache].yPrime = findYPrime(m_cache[nCache].cacheSample, yPrimeResults);
        m_cache[nCache].margin = yPrimeResults.confidence(m_cache[nCache].cacheSample.y) - yPrimeResults.confidence(m_cache[nCache].yPrime);
    }

    // Annealing
    m_nuP *= m_hp->annealingRate;
    m_nuD *= m_hp->annealingRate;

    // Primal/Dual gradient descent/ascent
    VectorXd q = VectorXd::Ones(m_hp->numBases, 1.0);
    vector<vector<double> > dG(m_hp->numBases);
    double e, z, nuThetaInv = m_nuD / m_hp->theta;
    for (int nBase = 0; nBase < m_hp->numBases; nBase++) {
        // Dual
        VectorXd sampleW(m_cache.size());
        for (int nCache = 0; nCache < (int) m_cache.size(); nCache++) {
            Result baseResult(*m_numClasses);
            m_bases[nBase]->update(m_cache[nCache].cacheSample);
            m_bases[nBase]->eval(m_cache[nCache].cacheSample, baseResult);
            dG[nBase].push_back(baseResult.confidence(m_cache[nCache].cacheSample.y) -
                                baseResult.confidence(m_cache[nCache].yPrime));

            sampleW(nCache) = m_cache[nCache].cacheSample.w;
            e = 0.0;
            for (int nBase1 = 0; nBase1 < nBase; nBase1++) {
                if (q(nBase1) < 0.0) {
                    e += nuThetaInv * q(nBase1) * dG[nBase1][nCache];
                }
            }
            sampleW(nCache) = max(0.0, min(m_hp->C, sampleW(nCache) + m_nuD + e));
        }

        // Primal
        z = 0.0;
        for (int nCache = 0; nCache < (int) m_cache.size(); nCache++) {
            z += m_cache[nCache].cacheSample.w * dG[nBase][nCache];
        }
        z = m_w(nBase) - m_nuP * (1.0 - z);
        m_w(nBase) = max(0.0, z);

        // Slack update
        q(nBase) = 1.0 - m_hp->theta * m_w(nBase);
        for (int nCache = 0; nCache < (int) m_cache.size(); nCache++) {
            m_cache[nCache].cacheSample.w = sampleW(nCache);
            q(nBase) -= m_cache[nCache].cacheSample.w * dG[nBase][nCache];
        }
    }
}

int OnlineMCLPBoost::findYPrime(const Sample& sample, const Result& result) {
    int yPrime = 0;
    double maxF = -1e6;
    for (int nClass = 0; nClass < *m_numClasses; nClass++) {
        if (nClass != sample.y && result.confidence(nClass) > maxF) {
            maxF = result.confidence(nClass);
            yPrime = nClass;
        }
    }
    return yPrime;
}
