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

#include "linear_larank.h"

LinearLaRank::LinearLaRank(const Hyperparameters& hp, const int& numClasses, const int& numFeatures,
                           const VectorXd& minFeatRange, const VectorXd& maxFeatRange) :
    Classifier(hp, numClasses), m_sampleCount(0), m_numFeatures(&numFeatures) {
    m_svm = new LaRank();
    m_svm->tau = 0.0001;
    m_svm->C = hp.larankC;
    m_name = "LinearLaRank";
}

LinearLaRank::~LinearLaRank() {
    delete m_svm;
}

void LinearLaRank::update(Sample& sample) {
    // Convert the Sample to LaRank form
    SVector laX;
    for (int nFeat = 0; nFeat < sample.x.rows(); nFeat++) {
        laX.set(nFeat, sample.x(nFeat));
    }

    // Add the sample to svm
    m_sampleCount++;
    m_svm->add(laX, sample.y, m_sampleCount, sample.w);
}

void LinearLaRank::eval(Sample& sample, Result& result) {
    // Evaluate the sample
    if (m_sampleCount) {
        // Convert the Sample to LaRank form
        SVector laX;
        for (int nFeat = 0; nFeat < sample.x.rows(); nFeat++) {
            laX.set(nFeat, sample.x(nFeat));
        }

        m_svm->predict_with_scores(laX, result);

        // Convert the scores to probabilities
        double totalProb = 0.0;
        for (int nClass = 0; nClass < *m_numClasses; nClass++) {
            result.confidence[nClass] = exp(result.confidence[nClass]);
            totalProb += result.confidence[nClass];
        }
        for (int nClass = 0; nClass < *m_numClasses; nClass++) {
            result.confidence[nClass] /= (totalProb + 1e-16);
        }
    } else {
        for (int nClass = 0; nClass < *m_numClasses; nClass++) {
            result.confidence[nClass] = 1.0 / *m_numClasses;
        }
        result.prediction = 0;
    }
}
