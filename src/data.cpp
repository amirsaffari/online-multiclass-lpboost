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

#include "data.h"

void DataSet::findFeatRange() {
    m_minFeatRange = VectorXd(m_numFeatures);
    m_maxFeatRange = VectorXd(m_numFeatures);

    double minVal, maxVal;
    for (int nFeat = 0; nFeat < m_numFeatures; nFeat++) {
        minVal = m_samples[0].x(nFeat);
        maxVal = m_samples[0].x(nFeat);
        for (int nSamp = 1; nSamp < m_numSamples; nSamp++) {
            if (m_samples[nSamp].x(nFeat) < minVal) {
                minVal = m_samples[nSamp].x(nFeat);
            }
            if (m_samples[nSamp].x(nFeat) > maxVal) {
                maxVal = m_samples[nSamp].x(nFeat);
            }
        }

        m_minFeatRange(nFeat) = minVal;
        m_maxFeatRange(nFeat) = maxVal;
    }
}

void DataSet::load(const string& x_filename, const string& y_filename) {
    ifstream xfp(x_filename.c_str(), ios::binary);
    if (!xfp) {
        cout << "Could not open input file " << x_filename << endl;
        exit(EXIT_FAILURE);
    }
    ifstream yfp(y_filename.c_str(), ios::binary);
    if (!yfp) {
        cout << "Could not open input file " << y_filename << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Loading data file: " << x_filename << " ... " << endl;

    // Reading the header
    int tmp;
    xfp >> m_numSamples;
    xfp >> m_numFeatures;
    yfp >> tmp;
    if (tmp != m_numSamples) {
        cout << "Number of samples in data and labels file is different" << endl;
        exit(EXIT_FAILURE);
    }
    yfp >> tmp;

    m_samples.clear();
    set<int> labels;
    for (int nSamp = 0; nSamp < m_numSamples; nSamp++) {
        Sample sample;
        sample.x = VectorXd(m_numFeatures);
        sample.id = nSamp;
        sample.w = 1.0;
        yfp >> sample.y;
        labels.insert(sample.y);
        for (int nFeat = 0; nFeat < m_numFeatures; nFeat++) {
            xfp >> sample.x(nFeat);
        }
        m_samples.push_back(sample); // push sample into dataset
    }
    xfp.close();
    yfp.close();
    m_numClasses = labels.size();

    // Find the data range
    findFeatRange();

    cout << "Loaded " << m_numSamples << " samples with " << m_numFeatures;
    cout << " features and " << m_numClasses << " classes." << endl;
}

Result::Result() {
}

Result::Result(const int& numClasses) : confidence(VectorXd::Zero(numClasses)) {
}

Cache::Cache() : margin(-1.0), yPrime(-1) {
}

Cache::Cache(const Sample& sample, const int& numBases, const int& numClasses) : margin(-1.0), yPrime(-1) {
    cacheSample.x = sample.x;
    cacheSample.y = sample.y;
    cacheSample.w = sample.w;
    cacheSample.id = sample.id;
}

