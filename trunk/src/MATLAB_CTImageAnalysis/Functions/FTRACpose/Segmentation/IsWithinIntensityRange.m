% $Id: IsWithinIntensityRange.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	IsWithinIntensityRange() 
% // file:		IsWithinIntensityRange.m
% // author:		siddharth
% // date:		
% // description:	checks whether the given point intensity lies within a
% certain given range
% 
% // input:	point intensity, Threshold value to use, range to look for
%           
% // output: true or false

% // calling function: LocateUpConnectedPoint(), LocateDownConnectedPoint,
% GetEdgeByGraphSearchTraversingUpBottom,GetEdgeByGraphSearchTraversingBott
% omUp

% // NOTE: 
% //-----------------------------------------------------------------------------
function [True] = IsWithinIntensityRange (iIntensity, iThreshold, iLowerRadius)

True = 0;
	if (iIntensity >= iThreshold)
		True = 1;
	else 
        if ((iThreshold - iIntensity) <= iLowerRadius) 
    		True = 1;
    	else 
            True = 0;
        end
    end
    
        
% $Log: IsWithinIntensityRange.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%