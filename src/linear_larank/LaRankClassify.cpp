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
            "\nLA_RANK_CLASSIFY: uses models learned with the 'LaRank algorithm' for multiclass classification to make prediction.\n"
            "!!! Only for models learned with the special implementation of LaRank for LINEAR kernel !!!\n"
            "\nUsage: la_rank_classify testing_set_file model_file prediction_file\n"
            );
    exit(1);
}

void testing(Machine* svm, Exampler test, char* pred_file)
{
    double err = 0;
    FILE* fp = fopen(pred_file, "w");
    std::cout << "\n--> Testing on " << test.nb_ex << "ex" << std::endl;
    for (int i = 0; i < test.nb_ex; i++) {
        int lab = test.data[i].cls;
        int predlab = svm->predict(test.data[i].inpt);
        if (predlab != lab)
            err++;
        fprintf(fp, "%d %d \n", lab, predlab);
    }
    fprintf(fp, "\n");
    fclose(fp);
    std::cout << "Test Error:" << 100 * (err / test.nb_ex) << "%" << std::endl;
}

Machine* load_model(char* file)
{
    Exampler model;
    model.libsvm_load_data(file, true);
    Machine* svm = create_larank();
    for (int i = 0; i < model.nb_ex; i++)
        svm->add_output(model.data[i].cls, LaFVector(model.data[i].inpt));

    return svm;
}

int main(int argc, char* argv[])
{
    Exampler test;
    int i;

    // parse options -- no option so far
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') break;
        ++i;
        switch (argv[i - 1][1]) {
        default:
            fprintf(stderr, "unknown option\n");
        }
    }

    // determine filenames
    if (i >= (argc - 2))
        exit_with_help();

    std::cout << "Loading Test Data" << std::endl;
    test.libsvm_load_data(argv[i], false);

    std::cout << "\nLoading Model" << std::endl;
    char* model_file = argv[i + 1];
    Machine* svm_load = load_model(model_file);

    char* pred_file = argv[i + 2];
    testing(svm_load, test, pred_file);

    delete svm_load;
    return 0;
}






