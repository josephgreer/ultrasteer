% $Id: DisplayFiducialSegmentation.m,v 1.5 2007/04/02 20:05:10 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplayFiducialSegmentationWholeImage() 
% // file:		DisplayFiducialSegmentationWholeImage.m
% // author:		siddharth
% // date:		
% // description:	display the results of fiducial segmentation overlaid on
% the whole image

% // assumed input:	
% variables that identify the region of interest coordinates
%           global FiducialROIXWidth; 
%           global FiducialROIYWidth;
%           global FiducialSegmentationROITranslationVector;
% variables for the results, 
%             global BLocations;
%             global lLine1PointsEq;
%             global lLine2PointsEq;
%             global lLine3PointsEq; 
%             global lEllipse1Points;
%             global lEllipse2Points;         
% 
% output: none
% 
% // calling function: called when Select region button is pressed, after
% performing segmentation,  after the 'Accept' button is pressed
% // NOTE: 
% //-----------------------------------------------------------------------------
function DisplayFiducialSegmentation()
    global FiducialSegmentation;
    
    ICopy = FiducialSegmentation.I;
    CurrentImageHandle = imshow(ICopy(FiducialSegmentation.ROI.StartX:FiducialSegmentation.ROI.StartX + FiducialSegmentation.ROI.SizeX, FiducialSegmentation.ROI.StartY:FiducialSegmentation.ROI.StartY + FiducialSegmentation.ROI.SizeY));
    
    hold on;

    Xoffset = FiducialSegmentation.ROI.StartX - 1;
    Yoffset = FiducialSegmentation.ROI.StartY - 1;

    % BBs
    for iBB = 1: length(FiducialSegmentation.BBs)
        if (FiducialSegmentation.BBs(iBB,1) ~= -1)
            plot(FiducialSegmentation.BBs(iBB,2) - Yoffset, FiducialSegmentation.BBs(iBB,1) - Xoffset, 'go');
            text(FiducialSegmentation.BBs(iBB,2) - Yoffset - 10, FiducialSegmentation.BBs(iBB,1) - Xoffset, num2str(iBB), 'color', 'g'); 
        end
    end
  
    % Lines
    if ~isempty(FiducialSegmentation.Line1)
        if (FiducialSegmentation.Line1(1) ~= -1) 
            if (FiducialSegmentation.Line1(2) ~= 0)
                X = [1 + Xoffset FiducialSegmentation.ROI.SizeX + Xoffset];
                Y = -((FiducialSegmentation.Line1(1) * X) + FiducialSegmentation.Line1(3)) / FiducialSegmentation.Line1(2);
                plot(Y - Yoffset, X - Xoffset, 'y-..');
                text(Y(1) - Yoffset, X(1)  - Xoffset + 5, 'Line 1', 'color', 'y');
            else
                % untested !!!
                Y = [1 FiducialSegmentation.ROI.SizeY];
                X = -FiducialSegmentation.Line1(3)/FiducialSegmentation.Line1(1);
                plot(Y - Yoffset, [X X] , 'y-..');
                text(Y(1) - Yoffset , X(1) + 5, 'Line 1', 'color', 'y');
            end        
        end
    end
    
    if ~isempty(FiducialSegmentation.Line2)
        if (FiducialSegmentation.Line2(1) ~= -1) 
            if (FiducialSegmentation.Line2(2) ~= 0)
                X = [1 + Xoffset FiducialSegmentation.ROI.SizeX + Xoffset];
                Y = -((FiducialSegmentation.Line2(1) * X) + FiducialSegmentation.Line2(3)) / FiducialSegmentation.Line2(2);
                plot(Y - Yoffset, X - Xoffset, 'y-..');
                text(Y(1) - Yoffset, X(1)  - Xoffset + 5, 'Line 2', 'color', 'y');
            else
                % untested !!!
                Y = [1 FiducialSegmentation.ROI.SizeY];
                X = -FiducialSegmentation.Line2(3)/FiducialSegmentation.Line2(1);
                plot(Y - Yoffset, [X X] , 'y-..');
                text(Y(1) - Yoffset , X(1) + 5, 'Line 2', 'color', 'y');
            end        
        end
    end
    
    if ~isempty(FiducialSegmentation.Line3)
        if (FiducialSegmentation.Line3(1) ~= -1) 
            if (FiducialSegmentation.Line3(2) ~= 0)
                X = [1 + Xoffset FiducialSegmentation.ROI.SizeX + Xoffset];
                Y = -((FiducialSegmentation.Line3(1) * X) + FiducialSegmentation.Line3(3)) / FiducialSegmentation.Line3(2);
                plot(Y - Yoffset, X - Xoffset, 'y-..');
                text(Y(1) - Yoffset, X(1)  - Xoffset + 5, 'Line 3', 'color', 'y');
            else
                % untested !!!
                Y = [1 FiducialSegmentation.ROI.SizeY];
                X = -FiducialSegmentation.Line3(3)/FiducialSegmentation.Line3(1);
                plot(Y - Yoffset, [X X] , 'y-..');
                text(Y(1) - Yoffset , X(1) + 5, 'Line 3', 'color', 'y');
            end        
        end
    end

    % Ellipses
    if ~isempty(FiducialSegmentation.Ellipse1)
        if (FiducialSegmentation.Ellipse1(1) ~= -1)
            ellipse1Eq = EllipseTranslate(FiducialSegmentation.Ellipse1, Xoffset, Yoffset);
            ellipse1Param = EllipseParamFromEq(ellipse1Eq);
            ellipse1Points = EllipsePointsFromParam(ellipse1Param);
            plot(ellipse1Points(2,:), ellipse1Points(1,:), 'c-.');
            text(ellipse1Param.Y0_in, ellipse1Param.X0_in, 'Ellipse 1', 'color', 'c');
        end
    end
    
    if ~isempty(FiducialSegmentation.Ellipse2)
        if (FiducialSegmentation.Ellipse2(1) ~= -1)
            ellipse2Eq = EllipseTranslate(FiducialSegmentation.Ellipse2, Xoffset, Yoffset);
            ellipse2Param = EllipseParamFromEq(ellipse2Eq);
            ellipse2Points = EllipsePointsFromParam(ellipse2Param);
            plot(ellipse2Points(2,:), ellipse2Points(1,:), 'c-.');
            text(ellipse2Param.Y0_in, ellipse2Param.X0_in, 'Ellipse 2', 'color', 'c');
        end
    end

    % Display user input
    if (~isempty(FiducialSegmentation.UserPoints))
        % final data set: [region/seed-number CGx CGy Orientation OrientationValidity PartOfCluster] 
        % note that the CGx is in image coordinate system, while plotting, x & y
        % should be swapped
        for iCG = 1:length(FiducialSegmentation.UserPoints(:,1))
            CG = FiducialSegmentation.UserPoints(iCG,1:2);
            plot(CG(1,2) - FiducialSegmentation.ROI.StartY + 1, CG(1,1) - FiducialSegmentation.ROI.StartX + 1, 'mx', 'MarkerSize', 12);
        end
    end
    
    hold off;



% $Log: DisplayFiducialSegmentation.m,v $
% Revision 1.5  2007/04/02 20:05:10  anton
% Fiducial segmentation: Added display of user input, existing points entered
% for lines and ellipses
%
% Revision 1.4  2007/01/10 17:20:41  anton
% DisplayFiducialSegmentation.m: Layout
%
% Revision 1.3  2007/01/10 03:22:28  anton
% DisplayFiducialSegmentation: Added support for ellipses and empty lines.
%
% Revision 1.2  2007/01/08 19:57:43  anton
% Segmentation: New code for fiducial segmentation.  Working version for
% lines, ellipses still not working.
%
% Revision 1.1  2007/01/03 22:25:05  anton
% Segmentation: Preliminary code for fiducial segmentation in separate window.  NOT WORKING YET.
%
% Revision 1.2  2006/04/20 20:40:34  anton
% Segmentation: DisplayXyzSegmentationWholeImage, use global flag to
% determine if should draw or not, tabify/format.
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
