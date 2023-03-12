#ifndef BasicModelExtractor_H
#define BasicModelExtractor_H

#include "ModelExtractor.h"
#include "ExtractedTeamModel.h"
#include "BasicTeamModel.h"
#include "TrajectoryData.h"
#include "BasicStateProcessor.h"
#include "Ordering.h"


class BasicModelExtractor : public ModelExtractor {
public:
    BasicModelExtractor(int _numK, int _numT, 
                        vector<string> &_teamCommState, 
                        vector<string> &_teamCommArgs);

    BasicModelExtractor(int _numK, int _numT, 
                        vector<string> &_teamCommState,
                        vector<string> &_teamCommArgs,
                        const vector<unique_ptr<ActionArgument>> &_allActionArguments, 
                        Ordering &_default_ordering);

    BasicTeamModel* extractModel(TrajectoryData &trajectories, BasicStateProcessor &state_processor);
    BasicTeamModel* extractModel(TrajectoryData &trajectories);

private:
    void loadDefaultOrdering();
    void loadDefaultArguments();
    void splitVec(vector<string> &allVecs, vector<string> &outputVecs, int vecID);
    vector<unique_ptr<ActionArgument>> allActionArguments;
    Ordering default_ordering;
    int numTakers;
    int numKeepers;
    vector<string> teamCommState;
    vector<string> teamCommArgs;
};

#endif
