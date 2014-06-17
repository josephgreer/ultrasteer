% $Id: WritePolyCoeffToFile.m,v 1.1 2006/03/07 20:35:03 gouthami Exp $
function WritePolyCoeffToFile(filename, polyOrder, polyCoeff)

% function WritePolyCoeffToFile(filename, polyOrder, polyCoeff)
% writes the bernstein polynomial coefficients to a file. These
% coefficients are calculated using dewarping script, and used to dewarp
% the images. The serialization order is
% 1. degree
% 2. Size of the polyCoeff matrix ( number of coefficients, num dimensions)
% 3. The actual coefficients
% param @filename - name of the file
% param @polyOrder - degree of the polynomial used
% param @polyCoeff - coefficients of the bernstein polynomial

fid = fopen(filename, 'wt');
fprintf(fid, '%d\n', polyOrder);
fprintf(fid, '%d\t%d\n', size(polyCoeff)');
fprintf(fid, '%f\t%f\n', polyCoeff');
fclose(fid);

% $Log: WritePolyCoeffToFile.m,v $
% Revision 1.1  2006/03/07 20:35:03  gouthami
% Adding to CVS
%