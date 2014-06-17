% $Id: CalculatePerimeterStats.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	CalculatePerimeterStats() 
% // file:		CalculatePerimeterStats.m
% // author:		siddharth
% // date:		
% // description:	calculates average perimeter and standard deviation from
% the population of seeds which is still unclassified.
% 
% // input:	Perimeters array, Array which indentifies which seeds/regions are
% still unclassfied
% 
% // output: Average perimeter, standard deviation
% 
% // calling function: BrachtherapySeedSegmentation
% 
% // NOTE: 
% //-----------------------------------------------------------------------------
function [AvgPerimeter, PerimeterDeviation] = CalculatePerimeterStats (Perimeter, IndicesOfUnclassifiedSeeds)
    PerimetersArray = [];
    for i=1: length(IndicesOfUnclassifiedSeeds)
        iRegionIndex = IndicesOfUnclassifiedSeeds(i);
        if(iRegionIndex > 0)
            PerimetersArray = [Perimeter(iRegionIndex); PerimetersArray];
        end
    end
    
    AvgPerimeter = mean(PerimetersArray);
    PerimeterDeviation = std(PerimetersArray);

    
%  $Log: CalculatePerimeterStats.m,v $
%  Revision 1.1  2005/10/27 19:10:47  svikal
%  Added CVS tags
%