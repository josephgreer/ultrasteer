function TFORMArray = ReadBernsteinTransformArray(fid, numFrames)
% function TFORMArray = ReadBernsteinTransformArray(fid, numFrames)
%
% Read a sequence of 'transform' objects from a file.  If 'numFrames' is
% specified, then that number of objects is read.  Otherwise, the number of
% frames is computed by reading a single object from the file's current
% (initial) position, then dividing the size of the file from the initial
% position to the end by the single object's size.  The function returns
% the array of transform objects.
%
initialFilePos = ftell(fid);
dummyTransform = ReadBernsteinImageTransform(fid);
transformSize = SizeOfBernsteinImageTransform(dummyTransform);
if (~exist('numFrames'))
    fseek(fid, 0, 'eof');
    eofFilePos = ftell(fid);
    numFrames = floor( (eofFilePos - initialFilePos) / transformSize );
end
fseek(fid, initialFilePos, 'bof');

for m=1:numFrames
    try,
        TFORMArray(m) = ReadBernsteinImageTransform(fid);
    catch,
        lastmsg = lasterr;
        error('In ReadBernsteinTransformArray, element %d: %s', m, lasterr);
    end
end
