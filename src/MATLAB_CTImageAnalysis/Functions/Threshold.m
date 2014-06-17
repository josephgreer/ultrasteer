% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: Threshold(x,numberFiles)
% -------------------------------------------------------------------------
% Inputs: x - image , numberFiles - (int) the number of files in the scan 
% folder.
% Outputs: image - (256x256xnumberFiles) thresholded image, v = isosurface 
% vertices.
% Sub_Functions Called: im2bw(), mat2gray(), isosurface()
% Synopsis:
%   This program will threshold the image to a user defined level to expose
%   the bone and needle. The threshold works by removing the lower desity
%   pixels in the CT scan, leaving only the pixels with higher density
%   than the threshold. 
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: Threshold()
% =========================================================================

function [image v] = Threshold(x,numberFiles)

% -------------------------------------------------------------------------
% Initialization of variables
% -------------------------------------------------------------------------

    ok = 0;

    image = zeros(256,256,numberFiles); %192 w imresize

    
% -------------------------------------------------------------------------
% Prompting user to pick threshold
% ------------------------------------------------------------------------- 
    
    while ok == 0
        level = input('Select Threshold Level (betweeen 0 and 1) : ')
        for i = 1:numberFiles
         image(:,:,i) = im2bw(mat2gray(x(:,:,i)),level);                   % Threshold with user given level.
        end
        isosurface(image);                                                 % Display isosurface

        confim = ThresholdingConfimGUI;                                    % Check if user is okay with thresholding with a dialog box

        if strcmp(confim,'Yes')                                            % Stop if they are, otherwise repeat. 
            ok = 1;
        end

    end

    [f v] = isosurface(image);                                             % When they are done, get the faces and vertices of the isosurface

end

% =========================================================================
% END FUNCTION: Threshold()
% =========================================================================

