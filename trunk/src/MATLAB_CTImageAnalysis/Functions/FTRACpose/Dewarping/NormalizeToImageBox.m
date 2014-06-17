% $Id: NormalizeToImageBox.m,v 1.1 2005/12/07 22:34:04 gouthami Exp $
function Mnorm = NormalizeToImageBox(M, rows, cols)
% function Mnorm = NormalizeToImageBox(M, rows, cols)
% NormalizeToImageBox normalizes the values of matrix M to a box defined by
% [1,1] and [rows,cols] as lower and upper bounds. [ rows, cols] represent
% the size of the image

% normalize the x and y values of M
Mnorm(:,1) = (M(:,1)-1)/(rows - 1);
Mnorm(:,2) = (M(:,2)-1)/(cols - 1);

%$Log: NormalizeToImageBox.m,v $
%Revision 1.1  2005/12/07 22:34:04  gouthami
%Adding the file to CVS for the first time
%