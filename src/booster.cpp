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

#include "booster.h"
#include "online_rf.h"
#include "linear_larank.h"

Booster::Booster(const Hyperparameters& hp, const int& numClasses, const int& numFeatures,
                 const VectorXd& minFeatRange, const VectorXd& maxFeatRange) :
    Classifier(hp, numClasses), m_w(VectorXd::Constant(hp.numBases, hp.shrinkage)) {
    switch (hp.weakLearner) {
    case WEAK_ORF: {
        OnlineRF *weakLearner;
        for (int nBase = 0; nBase < hp.numBases; nBase++) {
            weakLearner = new OnlineRF(hp, numClasses, numFeatures, minFeatRange, maxFeatRange);
            m_bases.push_back(weakLearner);
        }
        break;
    }
    case WEAK_LARANK: {
        LinearLaRank *weakLearner;
        for (int nBase = 0; nBase < hp.numBases; nBase++) {
            weakLearner = new LinearLaRank(hp, numClasses, numFeatures, minFeatRange, maxFeatRange);
            m_bases.push_back(weakLearner);
        }
        break;
    }
    }

}

Booster::~Booster() {
    for (int nBase = 0; nBase < m_hp->numBases; nBase++) {
        delete m_bases[nBase];
    }
}
    
void Booster::eval(Sample& sample, Result& result) {
    for (int nBase = 0; nBase < m_hp->numBases; nBase++) {
        Result baseResult(*m_numClasses);
        m_bases[nBase]->eval(sample, baseResult);
        baseResult.confidence *= m_w(nBase);
        result.confidence += baseResult.confidence;
    }
    result.confidence.maxCoeff(&result.prediction);
}
