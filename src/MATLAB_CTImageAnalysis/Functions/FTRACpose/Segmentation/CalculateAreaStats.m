% $Id: CalculateAreaStats.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	CalculateAreaStats() 
% // file:		CalculateAreaStats.m
% // author:		siddharth
% // date:		
% // description:	calculates average area and standard deviation from
% the population of seeds which is still unclassified.
% 
% // input:	Areas array, Array which indentifies which seeds/regions are
% still unclassfied
% 
% // output: Average area, standard deviation
% 
% // calling function: BrachtherapySeedSegmentation
% 
% // NOTE: 
% //-----------------------------------------------------------------------------
function [AvgArea, AreaDeviation] = CalculateAreaStats (Areas, IndicesOfUnclassifiedSeeds)
    AreasArray = [];
    for i=1: length(IndicesOfUnclassifiedSeeds)
        iRegionIndex = IndicesOfUnclassifiedSeeds(i);
        if(iRegionIndex > 0)
            AreasArray = [Areas(iRegionIndex); AreasArray];
        end
    end
    
    AvgArea = mean(AreasArray);
    AreaDeviation = std(AreasArray);

 
%  $Log: CalculateAreaStats.m,v $
%  Revision 1.1  2005/10/27 19:10:47  svikal
%  Added CVS tags
%