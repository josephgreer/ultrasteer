function [ a, b ] = readPolyFile( dir  )
%READPOLYFILE Reads the polynomial coefficients saved by Ultrasteer

rawPoly = csvread([dir 'polynomialCoefficients.txt']);

a = rawPoly(1:4);
b = rawPoly(5:8);

end

