function [header] = RPReadHeader(fid)
% loads the ultrasound RF data saved from the Sonix software
%
% Example
%       [Im, header] = RPread(filename, numFrames);
%
% Inputs:  
%       filename    The path of the data to open
%       numFrames   Number of frames to read, if this number is larger than
%       the total number of frames in the file it be rest to maximum number
%       of frames.
%
% Return:
%     Im            The image data returned into an array (h, w, numframes)
%     header        The file header information   
%
% Copyright Ultrasonix Medical corporation - Analogic Ultrasound Group
% Author: Reza Zahiri Azar, Jan 2012
%
% See also RunMe

if( fid == -1)
    error('Cannot open file');
end

% read the header info
hinfo = fread(fid, 19, 'int32');
header.filetype = hinfo(1);
header.nframes = hinfo(2);
header.w = hinfo(3);
header.h = hinfo(4);
header.ss = hinfo(5);
header.ul = [hinfo(6), hinfo(7)];
header.ur = [hinfo(8), hinfo(9)];
header.br = [hinfo(10), hinfo(11)];
header.bl = [hinfo(12), hinfo(13)];
header.probe = hinfo(14);
header.txf = hinfo(15);
header.sf = hinfo(16);
header.dr = hinfo(17);
header.ld = hinfo(18);
header.extra = hinfo(19);

