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


#include <cstdlib>
#include <sys/time.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cfloat>
#include <cassert>

#include "vectors.h"
#include "LaRank.h"
/////////////////////

void exit_with_help()
{
    fprintf(stdout,
            "\nLA_RANK_LEARN: learns models for multiclass classification with the 'LaRank algorithm'.\n"
            "!!! Implementation optimized for the use of LINEAR kernel only !!!\n"
            "\nUsage: la_rank_learn [options] training_set_file model_file\n"
            "options:\n"
            "-c cost : set the parameter C (default 1)\n"
            "-t tau : threshold determining tau-violating pairs of coordinates (default 1e-4) \n"
            "-m mode : set the learning mode (default 0)\n"
            "\t 0: online learning\n"
            "\t 1: batch learning (stopping criteria: duality gap < C)\n"
            "-v verbosity degree : display informations every v %% of the training set size (default 10)\n"
            );
    exit(1);
}

void training(Machine* svm, Exampler train, int step, int mode)
{
    int n_it = 0;
    double initime = getTime(), gap = DBL_MAX;

    std::cout << "\n--> Training on " << train.nb_ex << "ex" << std::endl;

    while (gap > svm->C) {
        n_it++;
        double tr_err = 0;
        int ind = step;
        for (int i = 0; i < train.nb_ex; i++) {
            int lab = train.data[i].cls;
            if (svm->add(train.data[i].inpt, lab, i) != lab)
                tr_err++;
            if (i / ind) {
                std::cout << "Done: " << (int) (((double) i) / train.nb_ex * 100) << "%, Train error (online): " << (tr_err / ((double) i + 1)) *100 << "%" << std::endl;
                svm->printStuff(initime, false);
                ind += step;
            }
        }

        std::cout << "---- End of iteration " << n_it << " ----" << std::endl;
        gap = svm->computeGap();
        std::cout << "Duality gap: " << gap << std::endl;
        std::cout << "Train error (online): " << (tr_err / train.nb_ex) *100 << "%" << std::endl;
        svm->printStuff(initime, true);
        if (mode == 0)
            gap = 0;
    }
    std::cout << "---- End of training ----" << std::endl;
}

void save_model(Machine* svm, const char* file)
{
    std::cout << "\n--> Saving Model in \"" << file << "\"" << std::endl;
    std::ofstream ostr(file);
    svm->save_outputs(ostr);
}

int main(int argc, char* argv[])
{

    Exampler train;
    int i, mode = 0;
    double C = 1, verb = 10, tau = 0.0001;

    // parse options
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') break;
        ++i;
        switch (argv[i - 1][1]) {
        case 'c':
            C = atof(argv[i]);
            break;
        case 't':
            tau = atof(argv[i]);
            break;
        case 'm':
            mode = atoi(argv[i]);
            break;
        case 'v':
            verb = atof(argv[i]);
            break;
        default:
            fprintf(stderr, "unknown option\n");
        }
    }

    // determine filenames
    if (i >= (argc - 1))
        exit_with_help();
    std::cout << "Loading Train Data " << std::endl;
    train.libsvm_load_data(argv[i], false);
    char* save_file = argv[i + 1];

    // CREATE
    int step = (int) ((double) train.nb_ex / (100 / verb));
    std::cout << "\n--> Building with C = " << C << std::endl;
    if (mode)
        std::cout << "    BATCH Learning" << std::endl;
    else
        std::cout << "    ONLINE Learning" << std::endl;

    Machine* svm = create_larank();

    svm->C = C;
    svm->tau = tau;

    training(svm, train, step, mode);
    save_model(svm, save_file);

    delete svm;
    return 0;
}
