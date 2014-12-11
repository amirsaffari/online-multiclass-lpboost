Online Multi-Class LPBoost
=========================
Amir Saffari <amir@ymer.org>

This is the original implementation of the Online Multi-Class LPBoost [1], Online Multi-Class Gradient Boost [1],
and Online Random Forest algorithms [2]. Read the INSTALL file for build instructions.

Usage:
======
Input arguments:
      -h | --help : will display this message.
      -c : \tpath to the config file.
      --ort : use Online Random Tree (ORT) algorithm.
      --orf : use Online Random Forest (ORF) algorithm.
      --omcb : use Online Multi-Class Gradient Boost (OMCBoost) algorithm.
      --omclp : use Online Multi-Class LPBoost (OMCLPBoost) algorithm.
      --larank : use Online LaRank algorithm.

      --train : train the classifier.
      --test : test the classifier.
      --t2 : train and test the classifier at the same time.

 Example:
      ./OMCBoost -c conf/omcb.conf --omclp --train --test

Config file:
============

All the settings for the classifier are passed via the config file. You can find the config file in "conf" folder. It is
easy to see what are the meanings behind each of these settings:

Data:
      * trainData = path to the training data (features)
      * trainLabels = path to the training labels
      * testData = path to the test data (features)
      * testLabels = path to test labels

Forest:
      * maxDepth = maximum depth for a tree
      * numRandomTests = number of random tests for each node
      * counterThreshold = number of samples to be seen for an online node before splitting
      * numTrees = number of trees in the forest

LaRank:

      * larankC = C regularization parameter for LaRank SVM

Boosting:
      * numBases = number of weak learners
      * weakLearner = type of weak learners: 0: ORF, 1: LaRank
      * shrinkage = shrinkage factor for gradient boost
      * lossFunction = type of loss function for gradient boost: 0 = Exponential Loss, 1 = Logit Loss
      * C = regularization parameter for LPBoost
      * cacheSize = size of the cache for LPBoost
      * nuD = dual gradient descent step size
      * nuP = primal gradient descent step size
      * annealingRate = rate of decay for step sizes during the training
      * theta = theta factor for the augmented Lagrangian
      * numIterations = number of iterations per sample

Experimenter:
      * findTrainError = find the training error over time
      * numEpochs = number of online training epochs

Output:
      * savePath = path to save the results (not implemented yet)
      * verbose = defines the verbosity level (0: silence)

Data format:
============

The data formats used is a simple ASCII format. Data is a represented as a matrix (vectors are matrices with 1
column). The first line in data file is the size of the matrix in form of: num_rows num_cols . From the next line the
matrix is written. You can find a few datasets in the data folder, check their header to see some examples. Currently,
there is only one limitation with the data files: the classes should be labeled starting in a regular format and start
from 0. For example, for a 3 class problem the labels should be in {0, 1, 2} set.

REFERENCES:
===========

[1] Amir Saffari, Martin Godec, Thomas Pock Christian Leistner, and Horst Bischof,
"Online Multi-Class LPBoost", in IEEE Conference on Computer Vision and Patter Recognition, 2010.
PDF: http://www.ymer.org/papers/files/2010-OMCLPBoost.pdf

[2] Amir Saffari, Christian Leistner, Jakob Santner, Martin Godec, and Horst Bischof,
"Online Random Forests", in 3rd IEEE ICCV Workshop on On-line Computer Vision, 2009.
PDF: http://www.ymer.org/papers/files/2009-OnlineRandomForests.pdf
