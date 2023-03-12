#ifndef ModelExtractor_H
#define ModelExtractor_H

#include <vector>
#include <memory>
#include "TrajectoryData.h"
#include "StateProcessor.h"
#include "ExtractedTeamModel.h"

using namespace std;

class ModelExtractor {
    virtual ExtractedTeamModel* extractModel(TrajectoryData &trajectories) = 0;
};

#endif