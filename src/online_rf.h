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

#ifndef ONLINERF_H_
#define ONLINERF_H_

#include "classifier.h"
#include "data.h"
#include "hyperparameters.h"
#include "utilities.h"

class RandomTest {
 public:
    RandomTest(const int& numClasses, const int& numFeatures, const VectorXd &minFeatRange, const VectorXd &maxFeatRange);

    void update(const Sample& sample);
    
    bool eval(const Sample& sample) const;
    
    double score() const;
    
    pair<VectorXd, VectorXd > getStats() const;
    
 protected:
    const int* m_numClasses;
    int m_feature;
    double m_threshold;
    
    double m_trueCount;
    double m_falseCount;
    VectorXd m_trueStats;
    VectorXd m_falseStats;

    void updateStats(const Sample& sample, const bool& decision);
};

class OnlineNode {
 public:
    OnlineNode(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, const VectorXd& minFeatRange, const VectorXd& maxFeatRange, 
            const int& depth);
    OnlineNode(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, const VectorXd& minFeatRange, const VectorXd& maxFeatRange, 
            const int& depth, const VectorXd& parentStats);
    
    ~OnlineNode();
    
    void update(const Sample& sample);
    void eval(const Sample& sample, Result& result);
    
 private:
    const int* m_numClasses;
    int m_depth;
    bool m_isLeaf;
    const Hyperparameters* m_hp;
    int m_label;
    double m_counter;
    double m_parentCounter;
    VectorXd m_labelStats;
    const VectorXd* m_minFeatRange;
    const VectorXd* m_maxFeatRange;
    
    OnlineNode* m_leftChildNode;
    OnlineNode* m_rightChildNode;
    
    vector<RandomTest*> m_onlineTests;
    RandomTest* m_bestTest;
    
    bool shouldISplit() const;
};


class OnlineTree: public Classifier {
 public:
    OnlineTree(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, const VectorXd& minFeatRange, const VectorXd& maxFeatRange);

    ~OnlineTree();
    
    virtual void update(Sample& sample);

    virtual void eval(Sample& sample, Result& result);
    
 private:
    OnlineNode* m_rootNode;
};


class OnlineRF: public Classifier {
 public:
    OnlineRF(const Hyperparameters& hp, const int& numClasses, const int& numFeatures, const VectorXd& minFeatRange, const VectorXd& maxFeatRange);

    ~OnlineRF();
    
    virtual void update(Sample& sample);

    virtual void eval(Sample& sample, Result& result);
    
 protected:
    double m_counter;
    double m_oobe;
    
    vector<OnlineTree*> m_trees;
};

#endif /* ONLINERF_H_ */
