% $Id: rufFiducialIsSegmented.m,v 1.1 2006/09/01 01:45:34 anton Exp $

% check if an image has been segmented for root, patient, batch and image
function fiducialIsSegmented = rufFiducialIsSegmented(directory, image)
    [pathstr, imagePrefix] = fileparts(image);
    BBsFile = length(dir(strcat(directory, '/FTRAC_Segmentation/BBs_', imagePrefix, '.txt')));
    Line1_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Line1_', imagePrefix, '.txt')));
    Line2_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Line2_', imagePrefix, '.txt')));
    Line3_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Line3_', imagePrefix, '.txt')));
    Ellipse1_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Ellipse1_', imagePrefix, '.txt')));
    Ellipse2_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Ellipse2_', imagePrefix, '.txt')));
    fiducialIsSegmented = ((BBsFile == 1) & (Line1_EqFile == 1) & (Line2_EqFile == 1) & (Line3_EqFile == 1) & (Ellipse1_EqFile == 1) & (Ellipse2_EqFile == 1));