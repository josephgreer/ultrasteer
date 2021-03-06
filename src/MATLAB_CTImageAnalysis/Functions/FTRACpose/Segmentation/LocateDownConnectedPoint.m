% $Id: LocateDownConnectedPoint.m,v 1.2 2007/08/03 22:40:30 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	LocateDownConnectedPoint() 
% // file:		LocateDownConnectedPoint.m
% // author:		siddharth
% // date:		
% // description:	from the given point, which is approx. BB center, find
% a point which is actually a connected point on the line, looking in
% downward direction..this is done by fitting a connectivity mask.. Since
% what is given is at the center, due to the characteric of looking at edge
% image, the given BB center is not going to have any connectivity (because
% it is inside a homogeneous region), therefore, we look vertically down 3
% point above the given points & look at 10-12 point below it. For each
% point find the connectivity score, score is actually being done for 'y'
% window i.e either 'y' row of points have highest ocnnectivity or 'y-1' or
% 'y+1'...then we pick 8th point above in highest connected row of points

% // input:	edge image, threshold value for point to be called connected, lowerthreshold range, Point to look for 
%           

% // output: bPointValid boolean flag that indicates whether a valid point
% has been found or not, DownConnectedPoint: the point found

% // calling function: ExtractLine()

% // NOTE: 
% //-----------------------------------------------------------------------------
function [bPointValid, DownConnectedPoint] = LocateDownConnectedPoint(PointToLookFor, iThreshold, iAcceptableThresholdRadius, Iedge)

	iX_Index = round(PointToLookFor (1,1));
	iY_Index = round(PointToLookFor (1,2));
    iX_Dimension = length(Iedge(:,1));
    iY_Dimension = length(Iedge(1,:));
	iPointIntensity = 0;	
	ConnectedPoints = 0;   
    bPointValid = true;

% 	//to check for
% 	// 1) x+3, y-1
% 	// 2) x+3, y
% 	// 3) x+3, y+1
% 	// 4) x+4, y-1
% 	// 5) x+4, y
% 	// 6) x+4, y+1
% 	// 7) x+5, y-1
% 	// 8) x+5, y
% 	// 9) x+5, y+1


% 1 y-1; 2 y; 3 y+1; 4 y-2; 5 y+2
IndividualScore = zeros(5,1);   


if (PointToLookFor(1,1) == 1)
    DownConnectedPoint = PointToLookFor;
    bPointValid = false;
    return;
end




% 	//check neighbourhood connections
% 	//1) x+3, y-1
	if ( (iX_Index + 3 < iX_Dimension) && iY_Index > 1)
		iPointIntensity = Iedge(iX_Index+3, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end        
    end
    
% 	//2) x+3, y
	if ( (iX_Index + 3 < iX_Dimension) && iY_Index > 0)
		iPointIntensity = Iedge(iX_Index+3, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end   
    end
 
% 	//3) x+3, y+1
	if ( (iX_Index + 3 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
		iPointIntensity = Iedge(iX_Index+3, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end 
    end

    % 	//3) x+3, y-2
	if ( (iX_Index + 3 < iX_Dimension) && iY_Index > 2)
		iPointIntensity = Iedge(iX_Index+3, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end     
    end

    % 	//3) x+3, y+2

	if ( (iX_Index + 3 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
		iPointIntensity = Iedge(iX_Index+3, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
    end

% 	//4) x+4, y-1
	if ( (iX_Index + 4 < iX_Dimension) && iY_Index > 1)
		iPointIntensity = Iedge(iX_Index+4, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end    
    end
    
% 	//5) x+4, y
	if ( (iX_Index + 4 < iX_Dimension) && iY_Index > 0)
		iPointIntensity = Iedge(iX_Index+4, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
    end
 
% 	//6) x+4, y+1
	if ( (iX_Index + 4 < iX_Dimension) && iY_Index + 1 < iY_Dimension)	
		iPointIntensity = Iedge(iX_Index+4, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end      
    end

% 	//6) x+4, y-2
	if ( (iX_Index + 4 < iX_Dimension) && iY_Index > 2)	
		iPointIntensity = Iedge(iX_Index+4, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end       
    end

    % 	//6) x+4, y+2
	if ( (iX_Index + 4 < iX_Dimension) && iY_Index + 2 < iY_Dimension)	
		iPointIntensity = Iedge(iX_Index+4, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
    end

% 	//7) x+5, y-1
	if ( (iX_Index + 5 < iX_Dimension) && iY_Index > 1)	
		iPointIntensity = Iedge(iX_Index+5, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end        
    end
    
% 	//8) x+5, y
	if ( (iX_Index + 5 < iX_Dimension) && iY_Index > 0)	
		iPointIntensity = Iedge(iX_Index+5, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end        
    end
 
% 	//9) x+5, y+1
	if ( (iX_Index + 5 < iX_Dimension) && iY_Index + 1 < iY_Dimension)	
		iPointIntensity = Iedge(iX_Index+5, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end        
    end

% 	//9) x+5, y-2
	if ( (iX_Index + 5 < iX_Dimension) && iY_Index > 2)	
		iPointIntensity = Iedge(iX_Index+5, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end        
    end

    % 	//9) x+5, y+2
	if ( (iX_Index + 5 < iX_Dimension) && iY_Index + 2 < iY_Dimension)	
		iPointIntensity = Iedge(iX_Index+5, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end        
    end

% 	//7) x+6, y-1

	if ( (iX_Index + 6 < iX_Dimension) && iY_Index > 1)
	
		iPointIntensity = Iedge(iX_Index+6, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end
        
    end
    
% 	//8) x+6, y

	if ( (iX_Index + 6 < iX_Dimension) && iY_Index > 0)
	
		iPointIntensity = Iedge(iX_Index+6, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
        
    end
 
% 	//9) x+6, y+1

	if ( (iX_Index + 6 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+6, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end
        
    end

% 	//9) x+6, y-2

	if ( (iX_Index + 6 < iX_Dimension) && iY_Index > 2)
	
		iPointIntensity = Iedge(iX_Index+6, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end
        
    end

    % 	//9) x+6, y+2

	if ( (iX_Index + 6 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+6, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
        
    end
    
% 	//7) x+7, y-1

	if ( (iX_Index + 7 < iX_Dimension) && iY_Index > 1)
	
		iPointIntensity = Iedge(iX_Index+7, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end
        
    end
    
% 	//8) x+7, y

	if ( (iX_Index + 7 < iX_Dimension) && iY_Index > 0)
	
		iPointIntensity = Iedge(iX_Index+7, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
        
    end
 
% 	//9) x+7, y+1

	if ( (iX_Index + 7 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+7, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end
        
    end

% 	//9) x+7, y-2

	if ( (iX_Index + 7 < iX_Dimension) && iY_Index > 2)
	
		iPointIntensity = Iedge(iX_Index+7, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end
        
    end

    % 	//9) x+7, y+2

	if ( (iX_Index + 7 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+7, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
        
    end
    
% 	//7) x+8, y-1

	if ( (iX_Index + 8 < iX_Dimension) && iY_Index > 1)
	
		iPointIntensity = Iedge(iX_Index+8, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end
        
    end
    
% 	//8) x+8, y

	if ( (iX_Index + 8 < iX_Dimension) && iY_Index > 0)
	
		iPointIntensity = Iedge(iX_Index+8, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
        
    end
 
% 	//9) x+8, y+1

	if ( (iX_Index + 8 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+8, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end
        
    end

% 	//9) x+8, y-2

	if ( (iX_Index + 8 < iX_Dimension) && iY_Index > 2)
	
		iPointIntensity = Iedge(iX_Index+8, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end
        
    end

    % 	//9) x+8, y+2

	if ( (iX_Index + 8 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+8, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
        
    end
    
% 	//7) x+9, y-1

	if ( (iX_Index + 9 < iX_Dimension) && iY_Index > 1)
	
		iPointIntensity = Iedge(iX_Index+9, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end
        
    end
    
% 	//8) x+9, y

	if ( (iX_Index + 9 < iX_Dimension) && iY_Index > 0)
	
		iPointIntensity = Iedge(iX_Index+9, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
        
    end
 
% 	//9) x+9, y+1

	if ( (iX_Index + 9 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+9, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end
        
    end

% 	//9) x+9, y-2

	if ( (iX_Index + 9 < iX_Dimension) && iY_Index > 2)
	
		iPointIntensity = Iedge(iX_Index+9, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end
        
    end

    % 	//9) x+9, y+2

	if ( (iX_Index + 9 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+9, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
        
    end
    
    % 	//7) x+10, y-1

	if ( (iX_Index + 10 < iX_Dimension) && iY_Index > 1)
	
		iPointIntensity = Iedge(iX_Index+10, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end
        
    end
    
% 	//8) x+10, y

	if ( (iX_Index + 10 < iX_Dimension) && iY_Index > 0)
	
		iPointIntensity = Iedge(iX_Index+10, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
        
    end
 
% 	//9) x+10, y+1

	if ( (iX_Index + 10 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+10, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end
        
    end

% 	//9) x+10, y-2

	if ( (iX_Index + 10 < iX_Dimension) && iY_Index > 2)
	
		iPointIntensity = Iedge(iX_Index+10, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end
        
    end

    % 	//9) x+10, y+2

	if ( (iX_Index + 10 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+10, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
        
    end

    
    % 	//7) x+11, y-1

	if ( (iX_Index + 11 < iX_Dimension) && iY_Index > 1)
	
		iPointIntensity = Iedge(iX_Index+11, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end
        
    end
    
% 	//8) x+11, y

	if ( (iX_Index + 11 < iX_Dimension) && iY_Index > 0)
	
		iPointIntensity = Iedge(iX_Index+11, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
        
    end
 
% 	//9) x+11, y+1

	if ( (iX_Index + 11 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+11, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end
        
    end

% 	//9) x+11, y-2

	if ( (iX_Index + 11 < iX_Dimension) && iY_Index > 2)
	
		iPointIntensity = Iedge(iX_Index+11, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end
        
    end

    % 	//9) x+11, y+2

	if ( (iX_Index + 11 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+11, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
        
    end

 % 	//7) x+12, y-1

	if ( (iX_Index + 12 < iX_Dimension) && iY_Index > 1)
	
		iPointIntensity = Iedge(iX_Index+12, iY_Index-1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(1) = IndividualScore(1) + 1; 
        end
        
    end
    
% 	//8) x+12, y

	if ( (iX_Index + 12 < iX_Dimension) && iY_Index > 0)
	
		iPointIntensity = Iedge(iX_Index+12, iY_Index);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(2) = IndividualScore(2) + 1; 
        end
        
    end
 
% 	//9) x+12, y+1

	if ( (iX_Index + 12 < iX_Dimension) && iY_Index + 1 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+12, iY_Index+1);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(3) = IndividualScore(3) + 1; 
        end
        
    end

% 	//9) x+12, y-2

	if ( (iX_Index + 12 < iX_Dimension) && iY_Index > 2)
	
		iPointIntensity = Iedge(iX_Index+12, iY_Index-2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(4) = IndividualScore(4) + 1; 
        end
        
    end

    % 	//9) x+12, y+2

	if ( (iX_Index + 12 < iX_Dimension) && iY_Index + 2 < iY_Dimension)
	
		iPointIntensity = Iedge(iX_Index+12, iY_Index+2);

		if( IsWithinIntensityRange(iPointIntensity, iThreshold, iAcceptableThresholdRadius))
			ConnectedPoints = ConnectedPoints + 1;
            IndividualScore(5) = IndividualScore(5) + 1; 
        end
        
    end

% 	if(ConnectedPoints >= 10)
%         DownConnectedPoint = [iX_Index+5 iY_Index-1];
%     else
%         DownConnectedPoint = [1 iY_Index-1];
%     end
if (max(IndividualScore) < 6)
    DownConnectedPoint(1,1) = 1;
    bPointValid = false; 
    return;
 
end

Index = find(IndividualScore == max(IndividualScore));

% if(length(Index) == 3)
%     Index(1) = Index(2);
% end
% Index (1) = max(Index);
if (Index(1) == 1)
    DownConnectedPoint = [(iX_Index+8) (iY_Index-1)];
end

if (Index(1) == 2)
    DownConnectedPoint = [(iX_Index+8) (iY_Index)];
end

if (Index(1) == 3)
    DownConnectedPoint = [(iX_Index+8) (iY_Index+1)];
end

if (Index(1) == 4)
    DownConnectedPoint = [(iX_Index+8) (iY_Index-2)];
end

if (Index(1) == 5)
    DownConnectedPoint = [(iX_Index+8) (iY_Index+2)];
end

% $Log: LocateDownConnectedPoint.m,v $
% Revision 1.2  2007/08/03 22:40:30  anton
% LocateUp/DownConnectedPoint.m: round input coordinates to allow
% re-entry after centroid adjustement.  Removed blank lines.
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
