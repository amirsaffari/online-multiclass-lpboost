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

#ifndef LINEARLARANK_H_
#define LINEARLARANK_H_

#include "classifier.h"
#include "data.h"
#include "hyperparameters.h"
#include "utilities.h"
#include "linear_larank/LaRank.h"
#include "linear_larank/vectors.h"

class LinearLaRank: public Classifier {
 public:
    LinearLaRank(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, const VectorXd& minFeatRange, const VectorXd& maxFeatRange);

    ~LinearLaRank();

    virtual void update(Sample& sample);
    virtual void eval(Sample& sample, Result& result);

 protected:
    Machine* m_svm;
    int m_sampleCount;
    const int* m_numFeatures;
};

#endif /* LINEARLARANK_H_ */
