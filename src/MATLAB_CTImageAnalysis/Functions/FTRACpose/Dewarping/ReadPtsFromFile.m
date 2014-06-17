% $Id: ReadPtsFromFile.m,v 1.1 2006/07/15 18:32:40 gouthami Exp $
function pts = ReadPtsFromFile(filename)
% Reads the point locations specified in the given file

fid = fopen(filename, 'rt');
if (fid < 0) 
    error ('Could not open  the coordinates file');
end

numBBs = str2num(fgetl(fid));

for i = 1:numBBs
    pts(i,:) = str2num(fgetl(fid));
end

fclose(fid);

% $Log: ReadPtsFromFile.m,v $
% Revision 1.1  2006/07/15 18:32:40  gouthami
% Adding to CVS
%