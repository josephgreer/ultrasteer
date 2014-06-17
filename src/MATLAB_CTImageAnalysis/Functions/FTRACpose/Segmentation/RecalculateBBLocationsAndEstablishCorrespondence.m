% //-----------------------------------------------------------------------------
% // function:	RecalculateBBLocationsAndEstablishCorrespondence() 
% // file:		RecalculateBBLocationsAndEstablishCorrespondence.m
% // author:	siddharth
% // date:		
% // description:  do two things
% 1) recalculate BB locations using line equations 
% 2) establish corespondence
% 1) recalculate BB locations: this is done by making the existing location
% to lie on the line (since, line is assumed to pass through the center of
% BB), then using interpolation along the line to get the minimum intensity
% point.
% 2) establish correspondence for the lines: this is done by our apriori
% knowledge of mechanical construction that if all the BBs are segmented,
% then the line 1 is the one that has passes through three BBs in the lower
% half of the image, line 2 is the one that passed through 2 BBs & line 3
% is the one that passes through 1 BB. There will be cases when BBs will
% not be completely segmented i.e. neither line 1 or line2 may actually pass through more
% than 1 detected BB, then in that case the distance from the 1st line is
% taken as a metric to say which line is which. 
% // input:	intensity image, line equations, BBlocations

% // output: line equations, BBlocations

% // calling function: during the process of Fiducial segementation, after
% pressing of the button Select region by FiducialSelectROI_Callback and
% after lines have been segmented

% // NOTE: There are multiple assumptions:
% //    1) We assume our line segmentation is more accurate than BB
% segmentation, thats why we recalculate BB positions by making them to lie
% on the line
% //    2) We assume that line passes through center of the BB
% //    3) we assume that BB center is the darkest pixel out of whole BB
% //    4) We assume that 3 BBs on the 1st line in bottom half of image will be segmented, otherwise
% there's no way of establishing correspondence
% //-----------------------------------------------------------------------
% ------

function [BBLocations, lLine1PointsEq, lLine2PointsEq, lLine3PointsEq, bLine1Found, bLine2Found, bLine3Found] = RecalculateBBLocationsAndEstablishCorrespondence(IROIFiducial, BBLocations,lLine1PointsEq, lLine2PointsEq, lLine3PointsEq, bLine1Found, bLine2Found, bLine3Found, Line1StartingPoint, Line2StartingPoint,Line3StartingPoint)

lActualLine1 = lLine1PointsEq;

lActualLine2 = lLine2PointsEq;

lActualLine3 = lLine3PointsEq;

Line1Score = 0;

Line2Score = 0;

Line3Score = 0;

iX_Dimension = length(IROIFiducial(:,1));

iY_Dimension = length(IROIFiducial(1,:));

% bLine1Found = false;
% 
% bLine2Found = false;
% 
% bLine3Found = false;

% look within a pixel window of 5x5 for each BB CG located,
% from the line equation, determine the region/points on the line where the
% BB can be located
% take out BB one at a time, try to put it on line eq., also keep the count
% on how many BBs fit into one line
for iBB = 1: length(BBLocations)

    bLiesOnLine = false;
    
    BrightestPoint = [];
    
 if( (BBLocations(iBB,1) > 0)  && (BBLocations (iBB,2)> 0))
    [bLiesOnLine BrightestPoint] = IsOnLine(IROIFiducial, BBLocations(iBB,:), lLine1PointsEq, bLine1Found);
    
    if (bLiesOnLine)
        if(BBLocations(iBB,1) > iX_Dimension/2)
            Line1Score = Line1Score + 1;
        end
        BBLocations(iBB,:) = BrightestPoint;
    else
        [bLiesOnLine BrightestPoint] = IsOnLine(IROIFiducial, BBLocations(iBB,:), lLine2PointsEq, bLine2Found);
        
        if (bLiesOnLine)
            if(BBLocations(iBB,1) > iX_Dimension/2)
                Line2Score = Line2Score + 1;
            end    
            BBLocations(iBB,:) = BrightestPoint;
            
        else
            [bLiesOnLine BrightestPoint] = IsOnLine(IROIFiducial, BBLocations(iBB,:), lLine3PointsEq, bLine3Found);
        
            if (bLiesOnLine)
                if(BBLocations(iBB,1) > iX_Dimension/2)
                    Line3Score = Line3Score + 1;
                end
                BBLocations(iBB, :) = BrightestPoint;
            else
                hello = 0;
%               throw the point?
            end
            
        end
        
    end
    
 end
 
end


Line1 = 1;
% correspondence of line
if(Line1Score == 3)
    lActualLine1 = lLine1PointsEq;
    Line1 = 1;
    bLine1Found = true;
else
    if(Line2Score == 3)
        lActualLine1 = lLine2PointsEq;
        Line1 = 2;
        bLine1Found = true;
    else
        if(Line3Score == 3)
            lActualLine1 = lLine3PointsEq;
            Line1 = 3;
            bLine1Found = true;
        else
        % line 1 not found
            bLine1Found = false;
            
        end
    end
    
end

if (bLine1Found)
    
    if(bLine2Found && bLine3Found)
%      case when all line were segmented
        if (Line1 == 1)
            if (Line2Score == 2)
                lActualLine2 = lLine2PointsEq;      
                lActualLine3 = lLine3PointsEq;
            else
                if(Line3Score == 2)
                    lActualLine2 = lLine3PointsEq;    
                    lActualLine3 = lLine2PointsEq;
                else
                    % calculate the distance of the lines from the 1st line
                    if( abs(lLine2PointsEq(3)) < abs(lLine3PointsEq(3)) )
                        lActualLine2 = lLine2PointsEq;      
                        lActualLine3 = lLine3PointsEq;
                    else
                        lActualLine2 = lLine3PointsEq;      
                        lActualLine3 = lLine2PointsEq;
                    end
                end
            end
        else 
            if(Line1 == 2)
                if (Line1Score == 2)
                    lActualLine2 = lLine1PointsEq;
                    lActualLine3 = lLine3PointsEq;
                else
                    if(Line3Score == 2)
                        lActualLine2 = lLine3PointsEq;
                        lActualLine2 = lLine1PointsEq;
                    else
                         % calculate the distance of the lines from the 1st line
                        if( abs(lLine2PointsEq(3)) < abs(lLine3PointsEq(3)) )
                            lActualLine2 = lLine1PointsEq;      
                            lActualLine3 = lLine3PointsEq;
                        else
                            lActualLine2 = lLine3PointsEq;      
                            lActualLine3 = lLine1PointsEq;
                        end
    
                    
                    end
                end
            
            else
                if (Line1Score == 2)
                    lActualLine2 = lLine1PointsEq;
                    lActualLine3 = lLine2PointsEq;          
                else
                    if(Line2Score == 2)
                        lActualLine2 = lLine2PointsEq;
                        lActualLine3 = lLine1PointsEq;
                    else
                         % calculate the distance of the lines from the 1st line
                        if( abs(lLine2PointsEq(3)) < abs(lLine3PointsEq(3)) )
                            lActualLine2 = lLine1PointsEq;      
                            lActualLine3 = lLine2PointsEq;
                        else
                            lActualLine2 = lLine2PointsEq;      
                            lActualLine3 = lLine1PointsEq;
                        end
    
                    end
                end
            
            
            end
        
        end
    
    else
        if(bLine2Found || bLine3Found)
%       at least 1 more line found      
            if (Line1 == 1)
                if(bLine2Found && (Line2Score <= 1))
                    lActualLine3 = lLine2PointsEq;
                    bLine2Found = false;
                    bLine3Found = true;
                elseif (bLine3Found && (Line3Score == 2))
                    lActualLine2 = lLine3PointsEq;          
                    bLine2Found = true;
                    bLine3Found = false;
                end 
                    
            elseif(Line1 == 2)
                if(bLine2Found && (Line1Score <= 1))
                    lActualLine3 = lLine1PointsEq;
                    bLine2Found = false;
                    bLine3Found = true;
                elseif (bLine3Found && (Line3Score == 2))
                    lActualLine2 = lLine3PointsEq;          
                    bLine2Found = true;
                    bLine3Found = false;
                end                 
                
            elseif(Line1 == 3)
                if(bLine2Found && (Line2Score <= 1))
                    lActualLine3 = lLine2PointsEq;
                    bLine2Found = false;
                    bLine3Found = true;
                elseif (bLine3Found && (Line1Score == 2))
                    lActualLine2 = lLine1PointsEq;          
                    bLine2Found = true;
                    bLine3Found = false;
                end  
                
            end 

        else
%       just 1 line found      
            
        end 
        
        
    end 
        
        
else
%   line1 not found  
    
end 


lLine1PointsEq = lActualLine1;
lLine2PointsEq = lActualLine2;
lLine3PointsEq = lActualLine3;




%  establish BB correspondence
NewBBLocations = [-1 -1; 
                  -1 -1; 
                  -1 -1; 
                  -1 -1; 
                  -1 -1;
                  -1 -1;
                  -1 -1;
                  -1 -1;
                  -1 -1];

% first line 1

if (bLine1Found)

    iPointsOnLowerHalfLine = 0;
    
    PointsOnLowerHalf = [];
    
    for iBB = 1: length(BBLocations)
        
        iPoint = BBLocations (iBB,:);
        
        iResult = lLine1PointsEq(1)*iPoint(1,1) + lLine1PointsEq(2)*iPoint(1,2) + lLine1PointsEq(3);

        if(abs(iResult) <= 0.001)
        %   the point lies on the line
            
            if(iPoint(1,1) > iX_Dimension/2)
                iPointsOnLowerHalfLine = iPointsOnLowerHalfLine + 1;
                PointsOnLowerHalf = [PointsOnLowerHalf;iPoint];
            else
                NewBBLocations(4,:) = iPoint;
            end
                        
        end
        
    end
    
    if(iPointsOnLowerHalfLine == 3)
        
        iFirstPointIndex = find(PointsOnLowerHalf(:,1) == max(PointsOnLowerHalf(:,1)));
        NewBBLocations(1,:) = PointsOnLowerHalf(iFirstPointIndex(1),:);
        
        PointsOnLowerHalf(iFirstPointIndex(1),:) = [];
        
        iSecondPointIndex = find(PointsOnLowerHalf(:,1) == max(PointsOnLowerHalf(:,1)));
        NewBBLocations(2,:) = PointsOnLowerHalf(iSecondPointIndex(1),:);
        
        PointsOnLowerHalf(iSecondPointIndex(1),:) = [];
        
        iThirdPointIndex = find(PointsOnLowerHalf(:,1) == max(PointsOnLowerHalf(:,1)));
        NewBBLocations(3,:) = PointsOnLowerHalf(iThirdPointIndex(1),:);
    
        
%         
%                 if( abs(FindDistance (Line1StartingPoint, iPoint)) <= 2 || abs(FindDistance (Line2StartingPoint, iPoint)) <= 2 || abs(FindDistance (Line3StartingPoint, iPoint)) <= 2 || (iPoint(1,1) < NewBBLocations(2,1)))
%                     NewBBLocations (3,:) = iPoint;
%                 else
%                     if (NewBBLocations (1,1) == -1)
%                         NewBBLocations (1,:) = iPoint;
%                     else
%                         if (iPoint (1,1) > NewBBLocations (1,1))
%                             iTempPoint = NewBBLocations (1,:);
%                             NewBBLocations (1,:) = iPoint;
%                             NewBBLocations (2,:) = iTempPoint;
%                         else
%                             NewBBLocations (2,:) = iPoint;
%                         end
%                         
%                     end
%                     
%                 end
                 
    end
    
end

if (bLine2Found)

    for iBB = 1: length(BBLocations)
        
        iPoint = BBLocations (iBB,:);
        
        iResult = lLine2PointsEq(1)*iPoint(1,1) + lLine2PointsEq(2)*iPoint(1,2) + lLine2PointsEq(3);

        if(abs(iResult) <= 0.001)
        %   the point lies on the line
            
            if(iPoint(1,1) > iX_Dimension/2)
                if( abs(FindDistance (Line1StartingPoint, iPoint)) <= 2 || abs(FindDistance (Line2StartingPoint, iPoint)) <= 2 || abs(FindDistance (Line3StartingPoint, iPoint)) <= 2 || (iPoint(1,1) < NewBBLocations(5,1)))
                    NewBBLocations (6,:) = iPoint;    
                else
                    NewBBLocations (5,:) = iPoint;
                end

            else
                NewBBLocations(7,:) = iPoint;
            end
                        
        end
        
    end
    
end

if (bLine3Found)

    for iBB = 1: length(BBLocations)
        
        iPoint = BBLocations (iBB,:);
        
        iResult = lLine3PointsEq(1)*iPoint(1,1) + lLine3PointsEq(2)*iPoint(1,2) + lLine3PointsEq(3);

        if(abs(iResult) <= 0.001)
        %   the point lies on the line
            if(iPoint(1,1) > iX_Dimension/2)
                NewBBLocations(8,:) = iPoint;
            else
                NewBBLocations(9,:) = iPoint;
            end
                        
        end
        
    end
    
end


BBLocations = NewBBLocations;