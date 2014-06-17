function objectSize = SizeOfBernsteinImageTransform(TFORM)
% function objectSize = SizeOfBernsteinImageTransform(TFORM)
%
% Returns the number of bytes required to store a Bernsten transform object
% in a binary file.

objectSize = numel(TFORM.tdata.outputPixelToModelAffineTransform) * 8 + ...
    numel(TFORM.tdata.centerRow) * 8 + numel(TFORM.tdata.centerCol) * 8 + ...
    1 + ...  % polyFamily is 1 byte
    numel(TFORM.tdata.degree) * 4 + ...
    numel(TFORM.tdata.coefficientsX) * 8 + numel(TFORM.tdata.coefficientsY) * 8;
