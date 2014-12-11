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

#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <libconfig.h++>

#include "data.h"
#include "experimenter.h"
#include "online_rf.h"
#include "linear_larank.h"
#include "online_mcboost.h"
#include "online_mclpboost.h"

using namespace std;
using namespace libconfig;

typedef enum {
    ORT, ORF, OMCBOOST, OMCLPBOOST, LARANK
} CLASSIFIER_TYPE;

//! Prints the interface help message
void help() {
    cout << endl;
    cout << "OMCBoost Classification Package:" << endl;
    cout << "Input arguments:" << endl;
    cout << "\t -h | --help : \t will display this message." << endl;
    cout << "\t -c : \t\t path to the config file." << endl << endl;
    cout << "\t --ort : \t use Online Random Tree (ORT) algorithm." << endl;
    cout << "\t --orf : \t use Online Random Forest (ORF) algorithm." << endl;
    cout << "\t --omcb : \t use Online MCBoost algorithm." << endl;
    cout << "\t --omclp : \t use Online MCLPBoost algorithm." << endl;
    cout << "\t --omclppd : \t use Online MCLPBoostPD algorithm." << endl;
    cout << "\t --larank : \t use Online LaRank algorithm." << endl;
    cout << endl << endl;
    cout << "\t --train : \t train the classifier." << endl;
    cout << "\t --test : \t test the classifier." << endl;
    cout << "\t --t2 : \t train and test the classifier at the same time." << endl;
    cout << endl << endl;
    cout << "\tExamples:" << endl;
    cout << "\t ./OMCBoost -c conf/orf.conf --orf --train --test" << endl;
}

int main(int argc, char *argv[]) {
    // Parsing command line
    string confFileName;
    int classifier = -1, doTraining = false, doTesting = false, doT2 = false, inputCounter = 1;

    if (argc == 1) {
        cout << "\tNo input argument specified: aborting." << endl;
        help();
        exit(EXIT_SUCCESS);
    }

    while (inputCounter < argc) {
        if (!strcmp(argv[inputCounter], "-h") || !strcmp(argv[inputCounter], "--help")) {
            help();
            return EXIT_SUCCESS;
        } else if (!strcmp(argv[inputCounter], "-c")) {
            confFileName = argv[++inputCounter];
        } else if (!strcmp(argv[inputCounter], "--ort")) {
            classifier = ORT;
        } else if (!strcmp(argv[inputCounter], "--orf")) {
            classifier = ORF;
        } else if (!strcmp(argv[inputCounter], "--omcb")) {
            classifier = OMCBOOST;
        } else if (!strcmp(argv[inputCounter], "--omclp")) {
            classifier = OMCLPBOOST;
        } else if (!strcmp(argv[inputCounter], "--larank")) {
            classifier = LARANK;
        } else if (!strcmp(argv[inputCounter], "--train")) {
            doTraining = true;
        } else if (!strcmp(argv[inputCounter], "--test")) {
            doTesting = true;
        } else if (!strcmp(argv[inputCounter], "--t2")) {
            doT2 = true;
        } else {
            cout << "\tUnknown input argument: " << argv[inputCounter];
            cout << ", please try --help for more information." << endl;
            exit(EXIT_FAILURE);
        }

        inputCounter++;
    }

    cout << "OnlineMCBoost Classification Package:" << endl;

    if (!doTraining && !doTesting && !doT2) {
        cout << "\tNothing to do, no training, no testing !!!" << endl;
        exit(EXIT_FAILURE);
    }

    if (doT2) {
        doTraining = false;
        doTesting = false;
    }

    // Load the hyperparameters
    Hyperparameters hp(confFileName);

    // Creating the train data
    DataSet dataset_tr, dataset_ts;
    dataset_tr.load(hp.trainData, hp.trainLabels);
    if (doT2 || doTesting) {
        dataset_ts.load(hp.testData, hp.testLabels);
    }

    // Calling training/testing
    Classifier* model = NULL;

    switch (classifier) {
    case ORT: {
        model = new OnlineTree(hp, dataset_tr.m_numClasses, dataset_tr.m_numFeatures,
                               dataset_tr.m_minFeatRange, dataset_tr.m_maxFeatRange);
        break;
    }
    case ORF: {
        model = new OnlineRF(hp, dataset_tr.m_numClasses, dataset_tr.m_numFeatures,
                             dataset_tr.m_minFeatRange, dataset_tr.m_maxFeatRange);
        break;
    }
    case OMCBOOST: {
        model = new OnlineMCBoost(hp, dataset_tr.m_numClasses, dataset_tr.m_numFeatures,
                                  dataset_tr.m_minFeatRange, dataset_tr.m_maxFeatRange);
        break;
    }
    case OMCLPBOOST: {
        model = new OnlineMCLPBoost(hp, dataset_tr.m_numClasses, dataset_tr.m_numFeatures,
                                    dataset_tr.m_minFeatRange, dataset_tr.m_maxFeatRange);
        break;
    }
    case LARANK: {
        model = new LinearLaRank(hp, dataset_tr.m_numClasses, dataset_tr.m_numFeatures,
                                 dataset_tr.m_minFeatRange, dataset_tr.m_maxFeatRange);
        break;
    }
    }

    if (model) {
        if (doT2) {
            trainAndTest(model, dataset_tr, dataset_ts, hp);
        }
        if (doTraining) {
            train(model, dataset_tr, hp);
        }
        if (doTesting) {
            test(model, dataset_ts, hp);
        }
    }

    // Preparing for exit
    delete model;

    return EXIT_SUCCESS;
}
