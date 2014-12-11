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

#ifndef BOOSTER_H
#define BOOSTER_H

#include "classifier.h"

class Booster : public Classifier {
 public:
    Booster(const Hyperparameters& hp, const int& numClasses, const int& numFeatures,
            const VectorXd& minFeatRange, const VectorXd& maxFeatRange);
    ~Booster();
    
    virtual void eval(Sample& sample, Result& result);

 protected:
    vector<Classifier*> m_bases;
    VectorXd m_w;
    vector<Cache> m_cache;
};

#endif // BOOSTER_H
