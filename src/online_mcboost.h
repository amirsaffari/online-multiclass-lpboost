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

#ifndef ONLINEMCBOOST_H_
#define ONLINEMCBOOST_H_

#include <cmath>

#include "classifier.h"
#include "booster.h"
#include "data.h"
#include "hyperparameters.h"
#include "utilities.h"

class OnlineMCBoost: public Booster {
 public:
    OnlineMCBoost(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, const VectorXd& minFeatRange, const VectorXd& maxFeatRange);

    virtual void update(Sample& sample);

 private:
    inline double d_exp(const double& fy) {
        return exp(-fy);
    }

    inline double d_logit(const double& fy) {
        return 1.0 / (1.0 + exp(fy));
    }
};

#endif /* ONLINEMCBOOST_H_ */
