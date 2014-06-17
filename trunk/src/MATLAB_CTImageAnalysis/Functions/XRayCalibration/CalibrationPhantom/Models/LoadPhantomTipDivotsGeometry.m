% LoadPhantomTipDivotsGeometry
% 
% Return two 6*3 matrix of the positions of bottoms of the conical divots
% on the proximal and distal plates in the calibration phantom.  These
% should be used for registering the phantom to a polaris marker mounted on
% the phantom.

proximalDivots = [...
   109.58      9.59     13.90;...
    46.49     99.69     13.95;...
   -63.09     90.11     13.90;...
  -109.58     -9.59     13.90;...
   -46.49    -99.69     13.89;...
    63.09    -90.11     13.91];

distalDivots = [...
   109.58      9.59    112.73;...
    46.49     99.69    112.73;...
   -63.09     90.11    112.74;...
  -109.58     -9.59    112.74;...
   -46.49    -99.69    112.75;...
    63.09    -90.11    112.75];

divotPositions = [proximalDivots; distalDivots];
