#ifndef __BinnedNLLH__
#define __BinnedNLLH__

#include <TestStatistic.h>
#include <BinnedPdfManager.h>
#include <SystematicManager.h>
#include <vector>
#include <QuadraticConstraint.h>

class BinnedNLLH : public TestStatistic{
 public:
    BinnedNLLH() : fCalculatedDataPdf(false) {}
    ~BinnedNLLH(){}

    double Evaluate();
    void   SetPdfManager(const BinnedPdfManager&);
    void   SetSystematicManager(const SystematicManager&);

    void   AddPdf(const BinnedPdf&);
    void   AddSystematic(Systematic);

    void AddSystematicConstraint(const QuadraticConstraint& constr_);
    void AddNormalisationConstraint(const QuadraticConstraint& constr_);

    void SetSystematicConstraint(size_t index_, const QuadraticConstraint& constr_);
    void SetNormalisationConstraint(size_t index_, const QuadraticConstraint& constr_);

    QuadraticConstraint GetSystematicConstraint(size_t index_)    const;
    QuadraticConstraint GetNormalisationConstraint(size_t index_)  const;

    void  BinData();
 private:
    BinnedPdfManager  fPdfManager;
    SystematicManager fSystematicManager;  

    std::vector<QuadraticConstraint> fSystematicConstraints;
    std::vector<QuadraticConstraint> fNormalisationConstraints;
    
    BinnedPdf fDataPdf;
    bool      fCalculatedDataPdf;
    
    
};
#endif
