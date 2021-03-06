#include <GridSearch.h>
#include <TestStatistic.h>
#include <iostream>
#include <sstream>
#include <BinnedPdf.h>
#include <FitResult.h>
#include <SystematicExceptions.h>

void 
GridSearch::SetMinima(const std::vector<double>& minima_){
    fMinima = minima_;
}

void 
GridSearch::SetMaxima(const std::vector<double>& maxima_){
    fMaxima = maxima_;
}

void
GridSearch::SetStepSizes(const std::vector<double>& steps_){
    fStepSizes = steps_;
}

std::vector<double> 
GridSearch::GetMaxima() const{
    return fMaxima;
}

std::vector<double> 
GridSearch::GetMinima() const{
    return fMinima;
}

std::vector<double>
GridSearch::GetStepSizes() const{
    return fStepSizes;
}

FitResult
GridSearch::Optimise(){
    // list of rates followed by list of systematics
    std::vector<double> bestFit;
    bestFit.resize(pTestStatistic -> GetNParams());
    fMinVal = 0;


    // Initialise LH space and count the number of grid steps
    AxisCollection statAxes;

    unsigned maxSteps = 1;
    std::vector<double> gridCounts;

    for(size_t i = 0; i < fMinima.size(); i++){
        size_t axisCounts = 1 + static_cast<size_t>((fMaxima.at(i) - fMinima.at(i)) / fStepSizes.at(i));
        maxSteps *= axisCounts;
        gridCounts.push_back(axisCounts);
    }

    for(size_t i = 0; i < fMinima.size(); i++){
        std::stringstream ss;
        ss << "param " << i;
        statAxes.AddAxis(PdfAxis(ss.str(), 
                               fMinima.at(i) - fStepSizes.at(i)/2, 
                               fMaxima.at(i) - fStepSizes.at(i)/2,
                               gridCounts.at(i)
                               )
                       );
    }

    BinnedPdf statSpace(statAxes);
    
    // start at min value
    fParams = fMinima;
    
    // count interations
    unsigned stepCount = 0;
    
    while(Increment(0)){
        // calculate the new value
        // if bigger, grab this as new best fit

        if(!(stepCount++ %1000000)){
            std::cout << stepCount << " / " << maxSteps  << "\t"
                      <<  100 * double(stepCount)/maxSteps 
                      << "%" << std::endl;
        }

        pTestStatistic->SetParams(fParams);
        double currentVal = pTestStatistic->Evaluate();
        if (currentVal < fMinVal || !fMinVal){
                fMinVal = currentVal;
                bestFit = fParams;
        }
        
        // Save to LH space
        statSpace.Fill(fParams, currentVal);
    } 
    fFitResult.SetBestFit(bestFit);
    fFitResult.SetStatSpace(&statSpace);
    return fFitResult;
}


bool 
GridSearch::Increment(size_t index_){
    fParams[index_] += fStepSizes.at(index_);

    // wrap around past the maximum
    if (fParams[index_] > fMaxima.at(index_)){
        fParams[index_] = fMinima.at(index_);

        // if its the last index no rippling to do
        if (index_ == fStepSizes.size() - 1)
            return false;

        // ripple up to next index
        if (Increment(index_ + 1)) // propagate the false down
            return true;
        else
            return false;

    }
    return true;
}

FitResult
GridSearch::GetFitResult() const{
    return fFitResult;
}
