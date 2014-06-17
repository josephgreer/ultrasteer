% $Id: ImageAxesClick_Callback.m,v 1.9 2006/09/22 21:55:40 anton Exp $

function ImageAxesClick_Callback(hObject, eventdata, handles)
% hObject   
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


global iGlobalTaskIdentifier;

% global variables for seed editing
global SeedSegmentationDataSet;

% SeedUndoTaskList = [TaskId X Y]; note X, Y are in display coordinate
% system, not our coordinate system
global I; 
global FiducialROIStartX; global FiducialROIStartY; global FiducialROIXWidth; global FiducialROIYWidth;
global ROI;

% global variables for fiducial editing
global Floating_BBLocations;

global Floating_lLine1PointsEq;global Floating_lLine2PointsEq;global Floating_lLine3PointsEq; 
global lLine1PointsEq;global lLine2PointsEq;global lLine3PointsEq; 
global bLine1Removed; global bLine2Removed; global bLine3Removed;
global NumOfPointsToDefineLine; global PointsOnLine;

global Floating_lEllipse1Points;global Floating_lEllipse2Points;
global lEllipse1Points;global lEllipse2Points;
global NumOfPointsToDefineEllipse; global PointsOnEllipse;
global bEllipse1Removed; global bEllipse2Removed;

global SearchClosestPoint;

handles = guidata(gcbo);
axes(handles.ImageDisplay);
CurrentAxesHandle = handles.ImageDisplay;

CurrentPoint = [];

CurrentPoint = get(CurrentAxesHandle, 'currentpoint');


switch(iGlobalTaskIdentifier)
    case (1), 
        set(gcf, 'pointer', 'watch');
%       add a seed
        XToAdd = round(CurrentPoint (1,1));
        YToAdd = round(CurrentPoint (1,2));
        
        if (SearchClosestPoint == 1)
            PointToAdd = LocateDarkestPoint(I(ROI.StartY:ROI.StartY + ROI.SizeY, ROI.StartX:ROI.StartX + ROI.SizeX), [YToAdd XToAdd]);
        else
            PointToAdd = [YToAdd XToAdd];
        end
        % final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
        SeedSegmentationDataSet = [SeedSegmentationDataSet; PointToAdd(1,1) + ROI.StartY - 1 PointToAdd(1,2) + ROI.StartX - 1 0 0 0];
        DisplaySeedSegmentationROI;
        set(handles.SeedNumberText, 'String', strcat('Seeds: ', num2str(size(SeedSegmentationDataSet, 1))));  
        set(gcf, 'pointer', 'crosshair');

    case (2), 
%       delete/remove a seed  
        set(gcf, 'pointer', 'watch');
        XToDelete = round(CurrentPoint (1,1));
        YToDelete = round(CurrentPoint (1,2));
        % final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
        ToFindCG = [];
        ToFindCG = [YToDelete + ROI.StartY - 1  XToDelete + ROI.StartX - 1];
        iSeedIndex = FindSeed(SeedSegmentationDataSet, ToFindCG);
        if (iSeedIndex > 0)
            DelCG = SeedSegmentationDataSet(iSeedIndex, 1:2);
            SeedSegmentationDataSet(iSeedIndex, :) = [];
            DisplaySeedSegmentationROI;
            set(handles.SeedNumberText, 'String', strcat('Seeds: ', num2str(size(SeedSegmentationDataSet, 1))));  
        else
            %   alert user that no seed in close vicinity found, try by getting closer to the seed you wish to delete
            msgbox('No seed in close vicinity found to delete, try again by clicking closer to the seed you wish to delete', 'Error deleting seed', 'warn', 'modal');    
        end
        set(gcf, 'pointer', 'crosshair');

    case (3),
%       add BB on Fiducial
        set(gcf, 'pointer', 'watch');
        XToAdd = CurrentPoint (1,1);
        YToAdd = CurrentPoint (1,2);

        if ( find(Floating_BBLocations(:,1) == -1) )
            
            PointToAdd = LocateDarkestPoint (I(FiducialROIStartX:FiducialROIStartX + FiducialROIXWidth, FiducialROIStartY:FiducialROIStartY + FiducialROIYWidth), [YToAdd XToAdd]);
            
            InsertIndex = find (Floating_BBLocations (:,1) == -1);
        
            Floating_BBLocations(InsertIndex(1),:) = PointToAdd;
            hold on;
    
            plot(PointToAdd(1,2), PointToAdd(1,1), 'b.', 'ButtonDownFcn', 'ImageAxesClick_Callback');
        
            hold off;
        else
            msgbox('There cannot be more than 9 BBs; please remove at least one BB to add one', 'Error adding', 'warn', 'modal');                
        end
        
        set(gcf, 'pointer', 'arrow');
        
    case (4),
%       delete BB on Fiducial  
        set(gcf, 'pointer', 'watch');
        XToDelete = round(CurrentPoint (1,1));
        YToDelete = round(CurrentPoint (1,2));
        % BB locations: [CGx CGy]
        ToFindBB = [];
        ToFindBB = [YToDelete XToDelete];
        iBBIndex = FindSeed(Floating_BBLocations, ToFindBB);

        if (iBBIndex > 0)
            DelBB = Floating_BBLocations (iBBIndex, 1:2);

            Floating_BBLocations (iBBIndex, :) = [-1 -1];            
            DisplayFiducialSegmentationROI;
        
        else
            %   alert user that no seed in close vicinity found, try by getting closer to the seed you wish to delete
            msgbox('No BB in close vicinity found to delete, try again by clicking closer to the BB you wish to delete', 'Error deleting seed', 'warn', 'modal');                           
        end

        set(gcf, 'pointer', 'arrow');
        
    case (5),
%       add line  
        
        if(~bLine1Removed && ~bLine2Removed && ~bLine3Removed)
            msgbox('There cannot be more than 3 lines; please remove at least one line to add one', 'Error adding', 'warn', 'modal');    
        else
            NumOfPointsToDefineLine = NumOfPointsToDefineLine - 1;
            
            PointsOnLine = [PointsOnLine; CurrentPoint(1,2) CurrentPoint(1,1)];
            
             
            if (NumOfPointsToDefineLine == 0)
                set(gcf, 'pointer', 'watch');
                
                x = PointsOnLine(1,1);
                y = PointsOnLine(1,2);

%                 dLine1Distance = abs(lLine1PointsEq(1)*x + lLine1PointsEq(2)*y + lLine1PointsEq(3));
%     
%                 dLine2Distance = abs(lLine2PointsEq(1)*x + lLine2PointsEq(2)*y + lLine2PointsEq(3));
%     
%                 dLine3Distance = abs(lLine3PointsEq(1)*x + lLine3PointsEq(2)*y + lLine3PointsEq(3));
%     
%                 MinDistance = min([dLine1Distance dLine2Distance dLine3Distance]);
    
%                 if(MinDistance <=5)
%         
                    if(bLine1Removed)
                        Floating_lLine1PointsEq = least_squares_line(PointsOnLine);
                        bLine1Removed = false;                        
                    else 
                        if(bLine2Removed)
                            Floating_lLine2PointsEq = least_squares_line(PointsOnLine);
                            bLine2Removed = false;
                       else 
                            if(bLine3Removed )
                                Floating_lLine3PointsEq = least_squares_line(PointsOnLine);
                                bLine3Removed = false;
                            end
                        end
                    
                    end
                
                    DisplayFiducialSegmentationROI;
                    
%                 end
                
                
                NumOfPointsToDefineLine = 5;
                
                PointsOnLine = [];
            
                set(gcf, 'pointer', 'arrow');
                
            end
            
        end
        
       

    case (6),
%       delete a line
       set(gcf, 'pointer', 'watch');
       bOperationCompleted = false;


       Y = round(CurrentPoint (1,1));
       X = round(CurrentPoint (1,2));

       LineDeleted = 0;
       
        % find the line on which the point lies
        % line 1
       if (~bLine1Removed)
           iResult = lLine1PointsEq(1)*X + lLine1PointsEq(2)*Y + lLine1PointsEq(3);

           if (abs(iResult) <= 3)
               Floating_lLine1PointsEq(1) = 0; Floating_lLine1PointsEq(2) = 0; Floating_lLine1PointsEq(3) = 0;   
               bLine1Removed = true;
               LineDeleted = 1;
               bOperationCompleted = true;
           end    
        
       end
            
       if (~bLine2Removed)
           iResult = lLine2PointsEq(1)*X + lLine2PointsEq(2)*Y + lLine2PointsEq(3);
    
           if (abs(iResult) <= 3)
               Floating_lLine2PointsEq(1) = 0; Floating_lLine2PointsEq(2) = 0; Floating_lLine2PointsEq(3) = 0;           
               bLine2Removed = true;
               LineDeleted = 2;
               bOperationCompleted = true;
           end
          
       end
      
      
       if (~bLine3Removed)
           iResult = lLine3PointsEq(1)*X + lLine3PointsEq(2)*Y + lLine3PointsEq(3);
        
           if (abs(iResult) <= 3)
               Floating_lLine3PointsEq(1) = 0; Floating_lLine3PointsEq(2) = 0; Floating_lLine3PointsEq(3) = 0;   
               bLine3Removed = true;
               LineDeleted = 3;
               bOperationCompleted = true;
           end
       
       end
   
       if (bOperationCompleted)
           DisplayFiducialSegmentationROI;
       else
          %   alert user that no seed in close vicinity found, try by getting closer to the seed you wish to delete
            msgbox('No Line in close vicinity found to delete, try again by clicking closer to the line you wish to delete', 'Error removing line', 'warn', 'modal');                       
       end
       
       set(gcf, 'pointer', 'arrow');

   case (7),
  %       add ellipse
       
        if(~bEllipse1Removed && ~bEllipse2Removed)
            msgbox('There cannot be more than 2 ellipse; please remove at least one ellipse to add one', 'Error adding', 'warn', 'modal');    
        else
            NumOfPointsToDefineEllipse = NumOfPointsToDefineEllipse - 1;
            
            PointsOnEllipse = [PointsOnEllipse; CurrentPoint(1,2) CurrentPoint(1,1)];
          
            if (NumOfPointsToDefineEllipse == 0)
                set(gcf, 'pointer', 'watch');
                X = PointsOnEllipse(1,2);
                Y = PointsOnEllipse(1,1);
   
                
%                 [Ellipse1 el_eq_1] = best_fit_ellipse(lEllipse1Points(:,2), lEllipse1Points(:,1));
%                 ExpRes1 = abs(el_eq_1(1)*X*X + el_eq_1(2)*X*Y + el_eq_1(3)*Y*Y + el_eq_1(4)*X + el_eq_1(5)*Y + el_eq_1(6));
%                 
%                 [Ellipse2 el_eq_2] = best_fit_ellipse(lEllipse2Points(:,2), lEllipse2Points(:,1));
%                 ExpRes2 = abs(el_eq_2(1)*X*X + el_eq_2(2)*X*Y + el_eq_2(3)*Y*Y + el_eq_2(4)*X + el_eq_2(5)*Y + el_eq_2(6));
%                 
%                 MinDistance = min([ExpRes1 ExpRes2])
          
%                 if (MinDistance <= 5)
           
                   if (bEllipse1Removed && ~bEllipse2Removed)
            %          add ellipse 1
                       bEllipse1Removed = false;
                       Floating_lEllipse1Points = PointsOnEllipse;
                   else
                       if (bEllipse2Removed && ~bEllipse1Removed)
        %              add ellipse 2  
                           bEllipse2Removed = false;
                           Floating_lEllipse2Points = PointsOnEllipse;
                       else
                           bEllipse1Removed = false;
                           Floating_lEllipse1Points = PointsOnEllipse;                         
                       end
                       
                   end
       
                   DisplayFiducialSegmentationROI;
           
%                end
                
               NumOfPointsToDefineEllipse = 7;
                
               PointsOnEllipse = [];
               
               set(gcf, 'pointer', 'arrow');
                
            end
            
        end
        

            
       
   case(8),
%       delete an ellipse

       set(gcf, 'pointer', 'arrow');
       
       X = round(CurrentPoint (1,1));
       Y = round(CurrentPoint (1,2));

       % see if the point belongs to the ellipse?
       ExpRes1 = 100;
       ExpRes2 = 100;
       
       if (~bEllipse1Removed)
           [Ellipse1 el_eq_1] = best_fit_ellipse(Floating_lEllipse1Points(:,2), Floating_lEllipse1Points(:,1));
           ExpRes1 = abs(el_eq_1(1)*X*X + el_eq_1(2)*X*Y + el_eq_1(3)*Y*Y + el_eq_1(4)*X + el_eq_1(5)*Y + el_eq_1(6));
       end

       if(~bEllipse2Removed)
           [Ellipse2 el_eq_2] = best_fit_ellipse(Floating_lEllipse2Points(:,2), Floating_lEllipse2Points(:,1));
           ExpRes2 = abs(el_eq_2(1)*X*X + el_eq_2(2)*X*Y + el_eq_2(3)*Y*Y + el_eq_2(4)*X + el_eq_2(5)*Y + el_eq_2(6));
       end

       MinDistance = min([ExpRes1 ExpRes2]);
       
       if (MinDistance <=1)
           
           if (ExpRes1 == MinDistance)
%              remove ellipse 1
               bEllipse1Removed = true;
               Floating_lEllipse1Points = [];
           else
%              remove ellipse 2  
               bEllipse2Removed = true;
               Floating_lEllipse2Points = [];
           end
       
           DisplayFiducialSegmentationROI;
       else
           msgbox('No ellipse in close vicinity found to delete, try again by clicking closer to the ellipse you wish to delete', 'Error removing line', 'warn', 'modal');           
         
       end
       set(gcf, 'pointer', 'arrow');
       
end

% $Log: ImageAxesClick_Callback.m,v $
% Revision 1.9  2006/09/22 21:55:40  anton
% Segmentation: Added option to look for closest point or not when adding
% seeds by hand
%
% Revision 1.8  2005/12/23 00:51:17  jain
% The five files from Siddharth that I got are added here. Also I checked if these files
% are from which version - looks like cvs does not change the dates of the files and
% hence the confusion. This version is stable with bad cases, but output is still to be
% taken care of - uiwait will be added in the next version.
%
% Revision 1.6  2005/11/28 19:45:14  anton
% General Update from Siddharth:
% *: Handling failure cases in fiducial segmentation, when only partial region
%    is selected as region of interest for fiducial...however, note that the
%    region should not be too small
% *: Now the BB numbers, line numbers and ellipse numbers are displayed after
%    establishing correspondence to have a visual check for user that
%    correspondence established is correct; Note tht these are displayed only
%    on whole image, not zoomed up region of interest, because there user can
%    make changes....so just on whole image after segmentation & after user
%    says he accepts the results
% *: Few cases prev. unhandled in user intervention in fiducial segmentation
%    validation
%
% Revision 1.5  2005/11/08 00:47:51  svikal
% BB correspondence after edit/user changes
%
% Revision 1.3  2005/11/05 22:56:12  svikal
% improved ellipse detection; changed the name of the function of fitting ellipse; hence file changed; also comments
%
% Revision 1.2  2005/10/31 19:40:43  svikal
% no message
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
