#include <PdfConverter.h>
#include <BinnedPdf.h>
#include <IntegrablePdf.h>
#include <AxisCollection.h>
#include <vector>
#include <PdfExceptions.h>
#include <iostream>
#include <algorithm>

BinnedPdf
PdfConverter::ToBinnedPdf(const IntegrablePdf& analytic_, const AxisCollection& axes_){
    if (analytic_.GetNDims() != axes_.GetNDimensions())
        throw DimensionError("PdfConverter::Dimensionality of Pdf doesn't match requested axes!");

    BinnedPdf binnedPdf(axes_);
    // Loop over bins
    std::vector<double> lows(axes_.GetNDimensions());
    std::vector<double> highs(axes_.GetNDimensions());
    for(size_t bin = 0; bin < axes_.GetNBins(); bin++){
        axes_.GetBinLowEdges(bin, lows);
        axes_.GetBinHighEdges(bin, highs);

        binnedPdf.AddBinContent(bin, analytic_.Integral(lows, highs));
    }
    
    return binnedPdf;
}

TH1D
PdfConverter::ToTH1D(const BinnedPdf& pdf_){
    if(pdf_.GetNDims() != 1)
        throw DimensionError("Only a 1D pdf can be represented by a TH1D");
 
    const PdfAxis& axis  = pdf_.GetAxes().GetAxis(0);
    const unsigned nBins = axis.GetNBins(); 
    TH1D rtHist("", "", nBins, axis.GetBinLowEdge(0), axis.GetBinHighEdge(nBins - 1));
    rtHist.SetDirectory(0);

    for(unsigned bin = 0; bin < nBins; bin++)
        rtHist.SetBinContent(bin+1, pdf_.GetBinContent(bin));
    
    return rtHist;                         
}

BinnedPdf
PdfConverter::Marginalise(const BinnedPdf& binnedPdf_, 
                          const std::vector<size_t>& indicies_){
    // check the pdf does contain the indicies asked for
    DataRepresentation dRep = binnedPdf_.GetDataRep();
    std::vector<size_t> oldDataRepIndicies = dRep.GetIndicies();

    if (!dRep.GetLength())
        throw DimensionError("Cant project out, pdf doesn't have indicies - set the data rep!");

    for(size_t i = 0; i < indicies_.size(); i++)
        if (std::find(oldDataRepIndicies.begin(), 
                      oldDataRepIndicies.end(), 
                      indicies_.at(i)) == oldDataRepIndicies.end()){
            throw DimensionError("PdfConverter: Tried to project pdf onto dimension it doesn't contain!");
        }
    
    std::vector<size_t> relativeIndicies = DataRepresentation(indicies_).GetRelativeIndicies(dRep);

    // Get the axes you are interested in,  in the order requested
    AxisCollection newAxes;
    for(size_t i = 0;  i < indicies_.size(); i++)
        newAxes.AddAxis(binnedPdf_.GetAxes().GetAxis(i));

    // New pdf
    BinnedPdf marginalisedPdf(newAxes);
    
    // Now loop over the bins in old and fill new pdfs
    for(size_t bin = 0; bin < binnedPdf_.GetNBins(); bin++){
        std::vector<size_t> oldIndicies = binnedPdf_.UnpackIndicies(bin);
        std::vector<size_t> newIndicies;

        for(size_t i = 0; i < relativeIndicies.size(); i++)
            newIndicies.push_back(oldIndicies.at(relativeIndicies.at(i)));
        
        
        size_t newBin = marginalisedPdf.FlattenIndicies(newIndicies);

        marginalisedPdf.AddBinContent(newBin, binnedPdf_.GetBinContent(bin));
    }
    return marginalisedPdf;
}

BinnedPdf
PdfConverter::Marginalise(const BinnedPdf& pdf_, size_t index_){
    return Marginalise(pdf_, std::vector<size_t> (1, index_));
}
