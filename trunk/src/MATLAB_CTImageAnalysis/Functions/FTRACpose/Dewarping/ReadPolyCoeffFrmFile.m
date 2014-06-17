function [polyOrder, polyCoeff] = ReadPolyCoeffFrmFile(filename)

% function [polyOrder, polyCoeff] = ReadPolyCoeffFrmFile(filename)
% reads the polynomial coefficients from the given file
% param @filename - file from which the polynomial coefficients are read
% param @polyOrder - degree of the polynomial
% param @polyCoeff - polynomial coefficients 
% Deserialization format is 
% 1. degree
% 2. size of the polyCoeff matrix (number of coefficients, number of
% dimensions
% 3. coefficients

fid = fopen(filename, 'rt');

polyOrder = fscanf(fid, '%d', 1);
nTerms = fscanf(fid, '%d', 1);
nDims = fscanf(fid, '%d', 1);

tempPolyCoeff = fscanf(fid, '%f', nTerms *nDims);
polyCoeff = reshape(tempPolyCoeff, [nDims, nTerms]);
polyCoeff = polyCoeff';

fclose(fid);
