// -*- C++ -*-
// Copyright (C) 2008- Antoine Bordes

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA


#ifndef LARANK_H_
#define LARANK_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <ext/hash_map>
#include <ext/hash_set>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cfloat>
#include <cassert>

#define STDEXT_NAMESPACE __gnu_cxx
#define std_hash_map STDEXT_NAMESPACE::hash_map
#define std_hash_set STDEXT_NAMESPACE::hash_set
#define VERBOSE_LEVEL 0

#include "vectors.h"
#include "../data.h"

#define jmin(a,b) (((a)<(b))?(a):(b))
#define jmax(a,b) (((a)>(b))?(a):(b))

// EXAMPLER: to read and store data and model files.

class Exampler {
public:

    struct example_t {
        // AMIR BEGINS
        // Added the sample weight
        example_t(SVector x, int y)
        : inpt(x), cls(y), w(1.0)
        {
        }
        example_t(SVector x, int y, double w)
        : inpt(x), cls(y), w(w)
        {
        }
        // AMIR ENDS

        example_t()
        {
        }

        SVector inpt;
        int cls;
        // AMIR BEGINS
        double w; // sample weight
        // AMIR ENDS
    };

    typedef std::vector< example_t> examples_t;

    // int libsvm_load_data(char *filename, bool model_file)
    // {
    //     int index;
    //     double value;
    //     int elements, i;
    //     FILE *fp = fopen(filename, "r");

    //     if (fp == NULL) {
    //         fprintf(stderr, "Can't open input file \"%s\"\n", filename);
    //         exit(1);
    //     } else {
    //         printf("loading \"%s\"..  \n", filename);
    //     }
    //     int msz = 0;
    //     elements = 0;
    //     while (1) {
    //         int c = fgetc(fp);
    //         switch (c) {
    //         case '\n':
    //             ++msz;
    //             elements = 0;
    //             break;
    //         case ':':
    //             ++elements;
    //             break;
    //         case EOF:
    //             goto out;
    //         default:
    //             ;
    //         }
    //     }
    // out:
    //     rewind(fp);
    //     max_index = 0;
    //     nb_labels = 0;
    //     for (i = 0; i < msz; i++) {
    //         int label;
    //         SVector v;
    //         fscanf(fp, "%d", &label);
    //         if ((int) label >= nb_labels) nb_labels = label;
    //         while (1) {
    //             int c;
    //             do {
    //                 c = getc(fp);
    //                 if (c == '\n') goto out2;
    //             } while (isspace(c));
    //             ungetc(c, fp);
    //             fscanf(fp, "%d:%lf", &index, &value);
    //             v.set(index, value);
    //             if (index > max_index) max_index = index;
    //         }

    //     out2:
    //         data.push_back(example_t(v, label));
    //     }
    //     fclose(fp);
    //     if (model_file)
    //         printf("-> classes: %d\n", msz);
    //     else
    //         printf("-> examples: %d features: %d labels: %d\n", msz, max_index, nb_labels);
    //     nb_ex = msz;
    //     return msz;
    // }

    // AMIR BEGINS
    // Loads data from Python Numpy
    void load_data(const double *inSamples, const int *inLabels, const double *inWeights,
                   const int& inNumSamples, const int& inNumFeatures, const int& inNumClasses)
    {
        data.clear();
        for (int nSamp = 0; nSamp < inNumSamples; nSamp++) {
            SVector dataVec;
            for (int nFeat = 0; nFeat < inNumFeatures; nFeat++) {
                dataVec.set(nFeat, inSamples[nSamp * inNumFeatures + nFeat]);
            }
            data.push_back(example_t(dataVec, inLabels[nSamp], inWeights[nSamp]));
        }
        nb_ex = inNumSamples;
        max_index = inNumFeatures;
        nb_labels = inNumClasses;
    }
    void load_data(const double *inSamples, const int *inLabels,
                   const int& inNumSamples, const int& inNumFeatures, const int& inNumClasses)
    {
        data.clear();
        for (int nSamp = 0; nSamp < inNumSamples; nSamp++) {
            SVector dataVec;
            for (int nFeat = 0; nFeat < inNumFeatures; nFeat++) {
                dataVec.set(nFeat, inSamples[nSamp * inNumFeatures + nFeat]);
            }
            data.push_back(example_t(dataVec, inLabels[nSamp]));
        }
        nb_ex = inNumSamples;
        max_index = inNumFeatures;
        nb_labels = inNumClasses;
    }
    // Loads SVM weight vector from Python Numpy
    void load_weights(const double *inW, const int& inNumFeatures, const int& inNumClasses)
    {
        data.clear();
        for (int nClass = 0; nClass < inNumClasses; nClass++) {
            SVector dataVec;
            for (int nFeat = 0; nFeat < inNumFeatures; nFeat++) {
                dataVec.set(nFeat, inW[nClass * inNumFeatures + nFeat]);
            }
            data.push_back(example_t(dataVec, nClass));
        }
        nb_ex = inNumClasses;
        max_index = inNumFeatures;
        nb_labels = inNumClasses;
    }
    // AMIR ENDS

    // ...
    examples_t data;
    int nb_ex;
    int max_index;
    int nb_labels;
};


// LARANKPATTERN: to keep track of the support patterns

class LaRankPattern {
public:

    // AMIR BEGINS
    // Added the sample weight
    LaRankPattern(int x_id, const SVector& x, int y, double w)
    : x_id(x_id), x(x), y(y), w(w)
    {
    }
    LaRankPattern(int x_id, const SVector& x, int y)
    : x_id(x_id), x(x), y(y), w(1.0)
    {
    }
    // AMIR ENDS

    LaRankPattern()
    : x_id(0)
    {
    }

    bool exists() const
    {
        return x_id >= 0;
    }

    void clear()
    {
        x_id = -1;
    }

    int x_id;
    SVector x;
    int y;
    // AMIR BEGINS
    double w; // sample weight
    // AMIR ENDS
};

// LARANKPATTERNS: the collection of support patterns

class LaRankPatterns {
public:

    LaRankPatterns()
    {
    }

    ~LaRankPatterns()
    {
    }

    void insert(const LaRankPattern& pattern)
    {
        if (!isPattern(pattern.x_id)) {
            if (freeidx.size()) {
                std_hash_set<unsigned>::iterator it = freeidx.begin();
                patterns[*it] = pattern;
                x_id2rank[pattern.x_id] = *it;
                freeidx.erase(it);
            } else {
                patterns.push_back(pattern);
                x_id2rank[pattern.x_id] = patterns.size() - 1;
            }
        } else {
            int rank = getPatternRank(pattern.x_id);
            patterns[rank] = pattern;
        }
    }

    void remove(unsigned i)
    {
        x_id2rank[patterns[i].x_id] = 0;
        patterns[i].clear();
        freeidx.insert(i);
    }

    bool empty() const
    {
        return patterns.size() == freeidx.size();
    }

    unsigned size() const
    {
        return patterns.size() - freeidx.size();
    }

    LaRankPattern& sample()
    {
        assert(!empty());
        while (true) {
            unsigned r = rand() % patterns.size();
            if (patterns[r].exists())
                return patterns[r];
        }
        return patterns[0];
    }

    unsigned getPatternRank(int x_id)
    {
        return x_id2rank[x_id];
    }

    bool isPattern(int x_id)
    {
        return x_id2rank[x_id] != 0;
    }

    LaRankPattern& getPattern(int x_id)
    {
        unsigned rank = x_id2rank[x_id];
        return patterns[rank];
    }

    unsigned maxcount() const
    {
        return patterns.size();
    }

    LaRankPattern & operator [](unsigned i)
    {
        return patterns[i];
    }

    const LaRankPattern & operator [](unsigned i) const
    {
        return patterns[i];
    }

private:
    std_hash_set<unsigned> freeidx;
    std::vector<LaRankPattern> patterns;
    std_hash_map<int, unsigned> x_id2rank;
};



// MACHINE: the thing we learn

class Machine {
public:
    virtual ~Machine()
    {
    };

    //MAIN functions for training and testing
    virtual int add(const SVector& x, int classnumber, int x_id) = 0;
    // AMIR BEGINS
    // Added sample weight for learning phase
    virtual int add(const SVector& x, int classnumber, int x_id, double w) = 0;
    // AMIR ENDS
    virtual int predict(const SVector& x) = 0;
    virtual void predict_with_scores(const SVector& x, Result& result) = 0;

    // Functions for saving and loading model
    virtual void save_outputs(std::ostream& ostr) = 0;
    // AMIR BEGINS
    // Used for saving the SVM weights back to Python Numpy
    virtual void get_weights(double* outW, const int inNumFeatures) = 0;
    // AMIR ENDS
    virtual void add_output(int y, LaFVector wy) = 0;

    // Information functions
    virtual void printStuff(double initime, bool dual) = 0;
    virtual double computeGap() = 0;

    std_hash_set<int> classes;

    unsigned class_count() const
    {
        return classes.size();
    }

    double C;
    double tau;
};

class LaRankOutput {
public:

    LaRankOutput()
    {
    }

    LaRankOutput(LaFVector& w)
    : wy(w)
    {
    }

    ~LaRankOutput()
    {
    }

    double computeGradient(const SVector& xi, int yi, int ythis)
    {
        return (yi == ythis ? 1 : 0) -computeScore(xi);
    }

    double computeScore(const SVector& x)
    {
        return dot(wy, x);
    }

    void update(const SVector& xi, double lambda, int xi_id)
    {
        wy.add(xi, lambda);
        double oldb = beta.get(xi_id);
        beta.set(xi_id, oldb + lambda);
    }

    void save_output(std::ostream& ostr, int ythis) const
    {
        ostr << ythis << " " << wy;
    }

    // AMIR BEGINS
    // Used for saving the SVM weights back to Python Numpy
    void get_weights(double* outW, int inNumFeatures, int ythis) const
    {
        for (int nFeat = 0; nFeat < inNumFeatures; nFeat++) {
            outW[ythis * inNumFeatures + nFeat] = wy.get(nFeat);
        }
    }
    // AMIR ENDS

    double getBeta(int x_id) const
    {
        return beta.get(x_id);
    }

    bool isSupportVector(int x_id) const
    {
        return beta.get(x_id) != 0;
    }

    int getNSV() const
    {
        int res = 0;
        for (int i = 0; i < beta.size(); i++)
            if (beta.get(i) != 0)
                res++;
        return res;
    }

    double getW2() const
    {
        return dot(wy, wy);
    }

private:
    // BETA: keep track of the bea value of each support vector (indicator)
    SVector beta;
    // WY: conains the prediction information
    LaFVector wy;

};


inline double getTime()
{
    struct timeval tv;
    struct timezone tz;
    long int sec;
    long int usec;
    double mytime;
    gettimeofday(&tv, &tz);
    sec = (long int) tv.tv_sec;
    usec = (long int) tv.tv_usec;
    mytime = (double) sec + usec * 0.000001;
    return mytime;
}

class LaRank : public Machine {
public:

    LaRank() : nb_seen_examples(0), nb_removed(0),
    n_pro(0), n_rep(0), n_opt(0),
    w_pro(1), w_rep(1), w_opt(1), dual(0)
    {
    }

    ~LaRank()
    {
    }

    // LEARNING FUNCTION: add new patterns and run optimization steps selected with dapatative schedule

    virtual int add(const SVector& xi, int yi, int x_id)
    {
        ++nb_seen_examples;
        // create a new output object if never seen this one before
        if (!getOutput(yi))
            outputs.insert(std::make_pair(yi, LaRankOutput()));

        LaRankPattern tpattern(x_id, xi, yi);
        LaRankPattern &pattern = (patterns.isPattern(x_id)) ? patterns.getPattern(x_id) : tpattern;

        // ProcessNew with the "fresh" pattern
        double time1 = getTime();
        process_return_t pro_ret = process(pattern, processNew);
        double dual_increase = pro_ret.dual_increase;
        dual += dual_increase;
        double duration = (getTime() - time1);
        double coeff = dual_increase / (10 * (0.00001 + duration));
        n_pro++;
        w_pro = 0.05 * coeff + (1 - 0.05) * w_pro;

        // ProcessOld & Optimize until ready for a new processnew
        // (Adaptative schedule here)
        for (;;) {
            double w_sum = w_pro + w_rep + w_opt;
            double prop_min = w_sum / 20;
            if (w_pro < prop_min)
                w_pro = prop_min;
            if (w_rep < prop_min)
                w_rep = prop_min;
            if (w_opt < prop_min)
                w_opt = prop_min;
            w_sum = w_pro + w_rep + w_opt;
            double r = rand() / (double) RAND_MAX * w_sum;
            if (r <= w_pro) {
                break;
            } else if ((r > w_pro) && (r <= w_pro + w_rep)) {
                double time1 = getTime();
                double dual_increase = reprocess();
                dual += dual_increase;
                double duration = (getTime() - time1);
                double coeff = dual_increase / (0.00001 + duration);
                n_rep++;
                w_rep = 0.05 * coeff + (1 - 0.05) * w_rep;
            } else {
                double time1 = getTime();
                double dual_increase = optimize();
                dual += dual_increase;
                double duration = (getTime() - time1);
                double coeff = dual_increase / (0.00001 + duration);
                n_opt++;
                w_opt = 0.05 * coeff + (1 - 0.05) * w_opt;
            }
        }
        if (nb_seen_examples % 100 == 0)
            nb_removed += cleanup();
        return pro_ret.ypred;
    }

    // AMIR BEGINS
    virtual int add(const SVector& xi, int yi, int x_id, double w)
    {
        ++nb_seen_examples;
        // create a new output object if never seen this one before
        if (!getOutput(yi))
            outputs.insert(std::make_pair(yi, LaRankOutput()));

        LaRankPattern tpattern(x_id, xi, yi, w);
        LaRankPattern &pattern = (patterns.isPattern(x_id)) ? patterns.getPattern(x_id) : tpattern;

        // ProcessNew with the "fresh" pattern
        double time1 = getTime();
        process_return_t pro_ret = process_w(pattern, processNew);
        double dual_increase = pro_ret.dual_increase;
        dual += dual_increase;
        double duration = (getTime() - time1);
        double coeff = dual_increase / (10 * (0.00001 + duration));
        n_pro++;
        w_pro = 0.05 * coeff + (1 - 0.05) * w_pro;

        // ProcessOld & Optimize until ready for a new processnew
        // (Adaptative schedule here)
        for (;;) {
            double w_sum = w_pro + w_rep + w_opt;
            double prop_min = w_sum / 20;
            if (w_pro < prop_min)
                w_pro = prop_min;
            if (w_rep < prop_min)
                w_rep = prop_min;
            if (w_opt < prop_min)
                w_opt = prop_min;
            w_sum = w_pro + w_rep + w_opt;
            double r = rand() / (double) RAND_MAX * w_sum;
            if (r <= w_pro) {
                break;
            } else if ((r > w_pro) && (r <= w_pro + w_rep)) {
                double time1 = getTime();
                double dual_increase = reprocess();
                dual += dual_increase;
                double duration = (getTime() - time1);
                double coeff = dual_increase / (0.00001 + duration);
                n_rep++;
                w_rep = 0.05 * coeff + (1 - 0.05) * w_rep;
            } else {
                double time1 = getTime();
                double dual_increase = optimize();
                dual += dual_increase;
                double duration = (getTime() - time1);
                double coeff = dual_increase / (0.00001 + duration);
                n_opt++;
                w_opt = 0.05 * coeff + (1 - 0.05) * w_opt;
            }
        }
        if (nb_seen_examples % 100 == 0)
            nb_removed += cleanup();
        return pro_ret.ypred;
    }
    // AMIR ENDS

    // PREDICTION FUNCTION: main function in la_rank_classify

    virtual int predict(const SVector& x)
    {
        int res = -1;
        double score_max = -DBL_MAX;
        for (outputhash_t::iterator it = outputs.begin(); it != outputs.end(); ++it) {
            double score = it->second.computeScore(x);
            if (score > score_max) {
                score_max = score;
                res = it->first;
            }
        }
        return res;
    }

    // AMIR BEGINS
    // Returns the confidence/label pair back
    virtual void predict_with_scores(const SVector& x, Result& result)
    {
        int res = -1, nClass = 0;
        double score_max = -DBL_MAX;
        for (outputhash_t::iterator it = outputs.begin(); it != outputs.end(); ++it, nClass++) {
            double score = it->second.computeScore(x);
            result.confidence[nClass] = score;
            if (score > score_max) {
                score_max = score;
                res = it->first;
            }
        }

        result.prediction = res;
    }
    // AMIR ENDS

    // Used for saving a model file

    virtual void save_outputs(std::ostream& ostr)
    {
        for (outputhash_t::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
            it->second.save_output(ostr, it->first);
    }

    // AMIR BEGINS
    // Used for saving the SVM weights back to Python Numpy
    virtual void get_weights(double* outW, const int inNumFeatures)
    {
        for (outputhash_t::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
            it->second.get_weights(outW, inNumFeatures, it->first);
        }
    }
    // AMIR ENDS

    // Used for loading a model file

    virtual void add_output(int y, LaFVector wy)
    {
        outputs.insert(std::make_pair(y, LaRankOutput(wy)));
    }

    // Compute Duality gap (costly but used in stopping criteria in batch mode)

    virtual double computeGap()
    {
        double sum_sl = 0;
        double sum_bi = 0;
        for (unsigned i = 0; i < patterns.maxcount(); ++i) {
            const LaRankPattern& p = patterns[i];
            if (!p.exists())
                continue;
            LaRankOutput* out = getOutput(p.y);
            if (!out)
                continue;
            sum_bi += out->getBeta(p.x_id);
            double gi = out->computeGradient(p.x, p.y, p.y);
            double gmin = DBL_MAX;
            for (outputhash_t::iterator it = outputs.begin(); it != outputs.end(); ++it) {
                if (it->first != p.y && it->second.isSupportVector(p.x_id)) {
                    double g = it->second.computeGradient(p.x, p.y, it->first);
                    if (g < gmin)
                        gmin = g;
                }
            }
            sum_sl += jmax(0, gi - gmin);
        }
        return jmax(0, getW2() + C * sum_sl - sum_bi);
    }

    // Display stuffs along learning

    virtual void printStuff(double initime, bool dual)
    {
        std::cout << "Current duration (CPUs): " << getTime() - initime << std::endl;
        if (dual)
            std::cout << "Dual: " << getDual() << " (w2: " << getW2() << ")" << std::endl;
        std::cout << "Number of Support Patterns: " << patterns.size() << " / " << nb_seen_examples << " (removed:" << nb_removed << ")" << std::endl;
        double w_sum = w_pro + w_rep + w_opt;
        std::cout << "ProcessNew:" << n_pro << " (" << w_pro / w_sum << ") ProcessOld:" << n_rep << " (" << w_rep / w_sum << ") Optimize:" << n_opt << " (" << w_opt / w_sum << ")" << std::endl;
        std::cout << "\t......" << std::endl;
    }

    virtual unsigned getNumOutputs() const
    {
        return outputs.size();
    }


private:
    /*
      MAIN DARK OPTIMIZATION PROCESSES
     */

    typedef std_hash_map<int, LaRankOutput> outputhash_t; // class index -> LaRankOutput
    outputhash_t outputs;

    LaRankOutput* getOutput(int index)
    {
        outputhash_t::iterator it = outputs.find(index);
        return it == outputs.end() ? NULL : &it->second;
    }

    const LaRankOutput* getOutput(int index) const
    {
        outputhash_t::const_iterator it = outputs.find(index);
        return it == outputs.end() ? NULL : &it->second;
    }

    LaRankPatterns patterns;

    int nb_seen_examples;
    int nb_removed;

    int n_pro;
    int n_rep;
    int n_opt;

    double w_pro;
    double w_rep;
    double w_opt;

    double dual;

    struct outputgradient_t {

        outputgradient_t(int output, double gradient)
        : output(output), gradient(gradient)
        {
        }

        outputgradient_t()
        : output(0), gradient(0)
        {
        }

        int output;
        double gradient;

        bool operator<(const outputgradient_t & og) const
        {
            return gradient > og.gradient;
        }
    };

    //3 types of operations in LaRank

    enum process_type {
        processNew,
        processOld,
        processOptimize
    };

    struct process_return_t {

        process_return_t(double dual, int ypred)
        : dual_increase(dual), ypred(ypred)
        {
        }

        process_return_t()
        {
        }
        double dual_increase;
        int ypred;
    };

    //Main optimization step

    process_return_t process(const LaRankPattern& pattern, process_type ptype)
    {
        process_return_t pro_ret = process_return_t(0, 0);
        std::vector<outputgradient_t> outputgradients;
        outputgradients.reserve(getNumOutputs());
        std::vector<outputgradient_t> outputscores;
        outputscores.reserve(getNumOutputs());

        //Compute gradient and sort
        for (outputhash_t::iterator it = outputs.begin(); it != outputs.end(); ++it)
            if (ptype != processOptimize || it->second.isSupportVector(pattern.x_id)) {
                double g = it->second.computeGradient(pattern.x, pattern.y, it->first);
                outputgradients.push_back(outputgradient_t(it->first, g));
                if (it->first == pattern.y)
                    outputscores.push_back(outputgradient_t(it->first, (1 - g)));
                else
                    outputscores.push_back(outputgradient_t(it->first, -g));
            }
        std::sort(outputgradients.begin(), outputgradients.end());


        //Determine the prediction and its confidence
        std::sort(outputscores.begin(), outputscores.end());
        pro_ret.ypred = outputscores[0].output;

        //Find yp
        outputgradient_t ygp;
        LaRankOutput* outp = NULL;
        unsigned p;
        for (p = 0; p < outputgradients.size(); ++p) {
            outputgradient_t& current = outputgradients[p];
            LaRankOutput* output = getOutput(current.output);
            bool support = ptype == processOptimize || output->isSupportVector(pattern.x_id);
            bool goodclass = current.output == pattern.y;

            if ((!support && goodclass) || (support && output->getBeta(pattern.x_id) < (goodclass ? C : 0))) {
                ygp = current;
                outp = output;
                break;
            }
        }
        if (p == outputgradients.size())
            return pro_ret;

        //Find ym
        outputgradient_t ygm;
        LaRankOutput* outm = NULL;
        int m;
        for (m = outputgradients.size() - 1; m >= 0; --m) {
            outputgradient_t& current = outputgradients[m];
            LaRankOutput* output = getOutput(current.output);
            bool support = ptype == processOptimize || output->isSupportVector(pattern.x_id);
            bool goodclass = current.output == pattern.y;
            if (!goodclass || (support && output->getBeta(pattern.x_id) > 0)) {
                ygm = current;
                outm = output;
                break;
            }
        }
        if (m < 0)
            return pro_ret;

        //Throw or insert pattern
        if ((ygp.gradient - ygm.gradient) < tau)
            return pro_ret;
        if (ptype == processNew)
            patterns.insert(pattern);

        //Compute lambda and clippe it
        double kii = dot(pattern.x, pattern.x);
        double lambda = (ygp.gradient - ygm.gradient) / (2 * kii);
        if (ptype == processOptimize || outp->isSupportVector(pattern.x_id)) {
            double beta = outp->getBeta(pattern.x_id);
            if (ygp.output == pattern.y)
                lambda = jmin(lambda, C - beta);
            else
                lambda = jmin(lambda, fabs(beta));
        } else
            lambda = jmin(lambda, C);

        //Update parameters
        outp->update(pattern.x, lambda, pattern.x_id);
        outm->update(pattern.x, -lambda, pattern.x_id);
        pro_ret.dual_increase = lambda * ((ygp.gradient - ygm.gradient) - lambda * kii);
        return pro_ret;
    }

    // AMIR BEGINS
    process_return_t process_w(const LaRankPattern& pattern, process_type ptype)
    {
        process_return_t pro_ret = process_return_t(0, 0);
        std::vector<outputgradient_t> outputgradients;
        outputgradients.reserve(getNumOutputs());
        std::vector<outputgradient_t> outputscores;
        outputscores.reserve(getNumOutputs());

        //Compute gradient and sort
        for (outputhash_t::iterator it = outputs.begin(); it != outputs.end(); ++it)
            if (ptype != processOptimize || it->second.isSupportVector(pattern.x_id)) {
                double g = it->second.computeGradient(pattern.x, pattern.y, it->first);
                outputgradients.push_back(outputgradient_t(it->first, g));
                if (it->first == pattern.y)
                    outputscores.push_back(outputgradient_t(it->first, (1 - g)));
                else
                    outputscores.push_back(outputgradient_t(it->first, -g));
            }
        std::sort(outputgradients.begin(), outputgradients.end());


        //Determine the prediction and its confidence
        std::sort(outputscores.begin(), outputscores.end());
        pro_ret.ypred = outputscores[0].output;

        //Find yp
        outputgradient_t ygp;
        LaRankOutput* outp = NULL;
        unsigned p;
        for (p = 0; p < outputgradients.size(); ++p) {
            outputgradient_t& current = outputgradients[p];
            LaRankOutput* output = getOutput(current.output);
            bool support = ptype == processOptimize || output->isSupportVector(pattern.x_id);
            bool goodclass = current.output == pattern.y;

            if ((!support && goodclass) || (support && output->getBeta(pattern.x_id) < (goodclass ? C * pattern.w : 0))) {
                ygp = current;
                outp = output;
                break;
            }
        }
        if (p == outputgradients.size())
            return pro_ret;

        //Find ym
        outputgradient_t ygm;
        LaRankOutput* outm = NULL;
        int m;
        for (m = outputgradients.size() - 1; m >= 0; --m) {
            outputgradient_t& current = outputgradients[m];
            LaRankOutput* output = getOutput(current.output);
            bool support = ptype == processOptimize || output->isSupportVector(pattern.x_id);
            bool goodclass = current.output == pattern.y;
            if (!goodclass || (support && output->getBeta(pattern.x_id) > 0)) {
                ygm = current;
                outm = output;
                break;
            }
        }
        if (m < 0)
            return pro_ret;

        //Throw or insert pattern
        if ((ygp.gradient - ygm.gradient) < tau)
            return pro_ret;
        if (ptype == processNew)
            patterns.insert(pattern);

        //Compute lambda and clippe it
        double kii = dot(pattern.x, pattern.x);
        double lambda = (ygp.gradient - ygm.gradient) / (2 * kii);
        if (ptype == processOptimize || outp->isSupportVector(pattern.x_id)) {
            double beta = outp->getBeta(pattern.x_id);
            if (ygp.output == pattern.y)
                lambda = jmin(lambda, C * pattern.w - beta);
            else
                lambda = jmin(lambda, fabs(beta));
        } else
            lambda = jmin(lambda, C * pattern.w);

        //Update parameters
        outp->update(pattern.x, lambda, pattern.x_id);
        outm->update(pattern.x, -lambda, pattern.x_id);
        pro_ret.dual_increase = lambda * ((ygp.gradient - ygm.gradient) - lambda * kii);
        return pro_ret;
    }
    // AMIR ENDS

    // 2nd optimization fiunction of LaRank (= ProcessOld in the paper)

    double reprocess()
    {
        if (patterns.size())
            for (int n = 0; n < 10; ++n) {
                process_return_t pro_ret = process(patterns.sample(), processOld);
                if (pro_ret.dual_increase)
                    return pro_ret.dual_increase;
            }
        return 0;
    }

    // 3rd optimization function of LaRank

    double optimize()
    {
        double dual_increase = 0;
        if (patterns.size())
            for (int n = 0; n < 10; ++n) {
                process_return_t pro_ret = process(patterns.sample(), processOptimize);
                dual_increase += pro_ret.dual_increase;
            }
        return dual_increase;
    }

    // remove patterns and return the number of patterns that were removed

    unsigned cleanup()
    {
        unsigned res = 0;
        for (unsigned i = 0; i < patterns.size(); ++i) {
            LaRankPattern& p = patterns[i];
            if (p.exists() && !outputs[p.y].isSupportVector(p.x_id)) {
                patterns.remove(i);
                ++res;
            }
        }
        return res;
    }


    /*
      INFORMATION FUNCTIONS: display/compute parameter values of the algorithm
     */

    // Number of Support Vectors

    int getNSV() const
    {
        int res = 0;
        for (outputhash_t::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
            res += it->second.getNSV();
        return res;
    }

    // Square-norm of the weight vector w

    double getW2() const
    {
        double res = 0;
        for (outputhash_t::const_iterator it = outputs.begin(); it != outputs.end(); ++it)
            res += it->second.getW2();
        return res;
    }

    // Square-norm of the weight vector w (another way of computing it, very costly)

    double computeW2()
    {
        double res = 0;
        for (unsigned i = 0; i < patterns.maxcount(); ++i) {
            const LaRankPattern& p = patterns[i];
            if (!p.exists())
                continue;
            for (outputhash_t::iterator it = outputs.begin(); it != outputs.end(); ++it)
                if (it->second.getBeta(p.x_id))
                    res += it->second.getBeta(p.x_id) * it->second.computeScore(p.x);
        }
        return res;
    }

    // DUAL objective value

    double getDual()
    {
        double res = 0;
        for (unsigned i = 0; i < patterns.maxcount(); ++i) {
            const LaRankPattern& p = patterns[i];
            if (!p.exists())
                continue;
            const LaRankOutput* out = getOutput(p.y);
            if (!out)
                continue;
            res += out->getBeta(p.x_id);
        }
        return res - getW2() / 2;
    }

};

#endif // !LARANK_H_
