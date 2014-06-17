% $Id: CalculateLengthStats.m,v 1.1 2005/10/27 19:10:47 svikal Exp $


% //-----------------------------------------------------------------------------
% // function:	CalculateLengthStats() 
% // file:		CalculateLengthStats.m
% // author:		siddharth
% // date:		
% // description:	calculates average length and standard deviation from
% the population of seeds which is still unclassified.
% 
% // input:	Lengths array, Array which indentifies which seeds/regions are
% still unclassfied
% 
% // output: Average length, standard deviation
% 
% // calling function: BrachtherapySeedSegmentation
% 
% // NOTE: 
% //-----------------------------------------------------------------------------
function [AvgLength, LengthDeviation] = CalculateLengthStats (MajorAxisLengths, IndicesOfUnclassifiedSeeds)
    LengthsArray = [];
    for i=1: length(IndicesOfUnclassifiedSeeds)
        iRegionIndex = IndicesOfUnclassifiedSeeds(i);
        if(iRegionIndex > 0)
            LengthsArray = [MajorAxisLengths(iRegionIndex); LengthsArray];
        end
    end
    
    AvgLength = mean(LengthsArray);
    LengthDeviation = std(LengthsArray);


%  $Log: CalculateLengthStats.m,v $
%  Revision 1.1  2005/10/27 19:10:47  svikal
%  Added CVS tags
%