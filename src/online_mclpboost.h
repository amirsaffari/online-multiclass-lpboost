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

#ifndef ONLINE_MCLPBOOST_H
#define ONLINE_MCLPBOOST_H

#include "classifier.h"
#include "booster.h"
#include "data.h"
#include "hyperparameters.h"
#include "utilities.h"

class OnlineMCLPBoost : public Booster {
 public:
    OnlineMCLPBoost(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, const VectorXd& minFeatRange, const VectorXd& maxFeatRange);

    virtual void update(Sample& sample);

 private:
    double m_nuD;
    double m_nuP;

    int findYPrime(const Sample& sample, const Result& result);
};

#endif // ONLINE_MCLPBOOST_H
