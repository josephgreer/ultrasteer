function TFORM = ReadBernsteinImageTransform(fid)
% function TFORM = ReadBernsteinImageTransform(fid)
%
% Read a single 'transform' object from a binary file (fid).  Return the
% object.
%

tdata.outputPixelToModelAffineTransform = fread(fid, [3 3], 'double')';
tdata.centerRow = fread(fid, 1, 'double');
tdata.centerCol = fread(fid, 1, 'double');
familyCode = fread(fid, 1, 'uint8');
tdata.degree = fread(fid, 1, 'int32');

if (familyCode == 0)
    tdata.polyFamily = 'tensor';
    [multinomialFactors, powersMatrix] = GenerateBivariateBernsteinTensorBasis(tdata.degree);
    numCoefficients = length(multinomialFactors);
elseif (familyCode == 1)
    tdata.polyFamily = 'barycentric';
    [multinomialFactors, powersMatrix] = GenerateCisstBernsteinBasis(3, tdata.degree);
    numCoefficients = length(multinomialFactors);
else
    error('ReadBernsteinImageTransform: Unknown polynomial family code');
end

tdata.coefficientsX = fread(fid, [numCoefficients, 1], 'double');
tdata.coefficientsY = fread(fid, [numCoefficients, 1], 'double');

% Create a 'transform' object
TFORM = maketform('custom', 2, 2, [], @BernsteinTransformPixels, tdata);

