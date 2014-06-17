% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: Segment(v)
% -------------------------------------------------------------------------
% Inputs: v - isosurface vertices
% Outputs: ROImatrix - (x,y,z) 3d matrix of x,y,z locations of points in
% the region of interest. 
% Sub_Functions Called: selectROI(), selectROILines()
% Synopsis:
%   This program will guide the user through selecting a region of
%   interest. The user can refine twice by using the selectROI() box
%   selection method and once with the selectROILine() line selection
%   method.
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: Segment()
% =========================================================================

function [ROImatrix] = Segment(v,inVivo)


% -------------------------------------------------------------------------
% Initial ROI Selection
% -------------------------------------------------------------------------
if inVivo == 1
    selectionStep = 8;
else
    selectionStep = 1;
end
ROImatrix = selectROI(v,selectionStep,0);
plot3(ROImatrix(:,1),ROImatrix(:,2),ROImatrix(:,3),'.','MarkerSize',4.5);
axis equal
uiwait

redo = ROIRedoSelectionGUI;

if strcmp(redo,'Yes')
    ROImatrix = selectROI(v,selectionStep,0);
    plot3(ROImatrix(:,1),ROImatrix(:,2),ROImatrix(:,3),'.','MarkerSize',4.5);   
    axis equal
end

% -------------------------------------------------------------------------
% Refining ROI Selection
% -------------------------------------------------------------------------
refine = ROIRefineSelectionGUI;

if strcmp(refine,'Yes')
    ROImatrix = selectROI(ROImatrix,1,0);                                   % USING BOX METHOD
    plot3(ROImatrix(:,1),ROImatrix(:,2),ROImatrix(:,3),'.','MarkerSize',4.5);
   % axis equal;
   refine = ROIRefineSelectionGUI;

    if strcmp(refine,'Yes')
        radius = 10;
        ROImatrix = selectROILines(ROImatrix,radius);                      % USING LINE METHOD
       % plot3(ROImatrix(:,1),ROImatrix(:,2),ROImatrix(:,3),'.','MarkerSize',4.5);
       % axis equal;
    end
end

% =========================================================================
% END FUNCTION: Segment()
% =========================================================================
