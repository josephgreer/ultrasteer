% $Id: FindThresholds.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	FindThresholds() 
% // file:		FindThresholds.m
% // author:		siddharth
% // date:		
% // description: calculate a simple threshold for the edge image.,
% histogram based; gives two maximum occuring pixel values separated by
% specified difference

% // input:	Image buffer, minimum difference between the thresholds

% // output: upper an dlower threshold values.

% // calling function: ExtractLine()

% // NOTE: 
% //-----------------------------------------------------------------------------
function [iUpperThreshold, iLowerThreshold] = FindThresholds(I, iMinDiffBetThresholds)
    
    iX_Dimension = length(I(:,1));
    
    iY_Dimension = length(I(1,:));

    szValArray = zeros(256,1);

	IntensityVal = 0;
		
	for  iX_Count = 1:iX_Dimension
	
        for iY_Count = 1: iY_Dimension
									
			IntensityVal = I(iX_Count, iY_Count);

            if(IntensityVal > 0)
                szValArray (IntensityVal) = szValArray(IntensityVal) + 1;
            end
                        
        end
        
    end
		
    iFirstMaxValue =  0;

	iMaxOccurance = 0;

	for  i = 45: 200
	
		if (szValArray (i) > iMaxOccurance)
		
			iMaxOccurance = szValArray(i);

			iFirstMaxValue = i;
        end
        
    end
	
	if ( rem(iFirstMaxValue, 10) < 5)
		iUpperThreshold = (iFirstMaxValue / 10)*10;
	else
		iUpperThreshold = (iFirstMaxValue / 5)*5;
    end

	
	iMaxOccurance = 0;

    iSecondMaxValue = 0;

	for i = 15: iFirstMaxValue
	
		if( (szValArray (i) > iMaxOccurance) && ((iFirstMaxValue - i) > iMinDiffBetThresholds))
		
			iMaxOccurance = szValArray(i);

			iSecondMaxValue = i;
            
        end
        
    end

	if ( rem(iSecondMaxValue, 10) < 5)
		iLowerThreshold = (iSecondMaxValue / 10)*10;
	else
		iLowerThreshold = (iSecondMaxValue / 5)*5;
    end
    

	if (iLowerThreshold < 20)
		iLowerThreshold = 15; 
    end
    
iUpperThreshold = round(iUpperThreshold);
iLowerThreshold = round(iLowerThreshold);

% $Log: FindThresholds.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%