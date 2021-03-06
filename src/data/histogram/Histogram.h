/*************************************************************************************/
/* An ND histogram is a combination of an axis collection that defines the binning   */
/* and a single vector of bin contents, indexed by the corresponding global bin ID.  */
/* Conversions between global bin ID and the indicies on each axis are performed by  */
/* the axis collection                                                               */
/*************************************************************************************/
#ifndef __OXSX_HISTOGRAM__
#define __OXSX_HISTOGRAM__
#include <AxisCollection.h>
#include <vector>

class Histogram{
 public:
    Histogram() : fNBins(0), fNDims(0) {}
    Histogram(const AxisCollection& axes_);
    Histogram(const Histogram& other_);
    
    double operator() (const std::vector<double>& vals_) const;
    double Integral()  const;
    void   Normalise();
    
    void   Fill(const std::vector<double>& vals_, double weight_ = 1);
    void   Fill(double val_, double weight_ = 1);

    size_t FindBin(const std::vector<double>& vals_) const;

    std::vector<size_t> UnpackIndicies(size_t bin_) const;
    size_t FlattenIndicies(const std::vector<size_t>& indicies_) const;

    const AxisCollection& GetAxes() const;
    void  SetAxes(const AxisCollection& axes_);

    std::vector<double> GetBinContents() const; 
    void SetBinContents(const std::vector<double>& data_);     

    std::vector<double> Means() const;
    std::vector<double> Variances() const;

    double GetBinContent(size_t bin_) const;
    size_t GetNBins() const;
    void   AddBinContent(size_t bin_, double content_);
    void   SetBinContent(size_t bin_, double content_);
    void   Empty();
    
 private:
    AxisCollection fAxes;
    std::vector<double> fBinContents;
    size_t fNBins;
    size_t fNDims;
};
#endif
