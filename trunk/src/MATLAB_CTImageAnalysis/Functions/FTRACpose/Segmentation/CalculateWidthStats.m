% $Id: CalculateWidthStats.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	CalculateWidthStats() 
% // file:		CalculateWidthStats.m
% // author:		siddharth
% // date:		
% // description:	calculates average width and standard deviation from
% the population of seeds which is still unclassified.
% 
% // input:	Widths array, Array which indentifies which seeds/regions are
% still unclassfied
% 
% // output: Average width, standard deviation
% 
% // calling function: BrachtherapySeedSegmentation
% 
% // NOTE: 
% //-----------------------------------------------------------------------------
function [AvgWidth, WidthDeviation] = CalculateWidthStats (MinorAxisLengths, IndicesOfUnclassifiedSeeds)
    WidthsArray = [];
    for i=1: length(IndicesOfUnclassifiedSeeds)
        iRegionIndex = IndicesOfUnclassifiedSeeds(i);
        if(iRegionIndex > 0)
            WidthsArray = [MinorAxisLengths(iRegionIndex); WidthsArray];
        end
    end
    
    AvgWidth = mean(WidthsArray);
    WidthDeviation = std(WidthsArray);

    
% $Log: CalculateWidthStats.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%