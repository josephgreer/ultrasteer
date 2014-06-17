function WriteBernsteinImageTransform(TFORM, fid)
% function WriteBernsteinImageTransform(TFORM, fid)
%
% Write the data elements of a Bernstein transform object in a binary
% format to the given file identifier.

fwrite(fid, TFORM.tdata.outputPixelToModelAffineTransform', 'double');
fwrite(fid, TFORM.tdata.centerRow', 'double');
fwrite(fid, TFORM.tdata.centerCol', 'double');
if (strcmp(TFORM.tdata.polyFamily, 'tensor'))
    fwrite(fid, 0, 'uint8');
else
    fwrite(fid, 1, 'uint8');
end
fwrite(fid, TFORM.tdata.degree, 'int32');
fwrite(fid, TFORM.tdata.coefficientsX, 'double');
fwrite(fid, TFORM.tdata.coefficientsY, 'double');
