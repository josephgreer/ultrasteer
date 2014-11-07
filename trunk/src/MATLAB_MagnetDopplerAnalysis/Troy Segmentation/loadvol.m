%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Loads the ultrasound volume data acquired using the Propello SDK
%%
%% Inputs:  
%%     datapath - The path of the data to open
%%     volN   - The volume to retrieve  (not used for now)
%%
%% Return:
%%     volume -  The volume data returned into a 3D array
%%     hdr - header which contains the following information:
%%
%%          hdr.datatype -   Data type (0: prescan B, 1: postscan B, 2: rf, 3: vel/var, 5 = B+flow RGB)
%%          hdr.numVols -    The number of volumes in the file
%%          hdr.fpV -        Frames per volume
%%          hdr.h -          Image height of the image (samples per line)
%%          hdr.w -          Image width (scan lines per frame)
%%          hdr.ss -         Sample size
%%
%% Example:
%%
%% [volume, hdr] = loadvol(datapath, 1);
%% 
%% for i = 1:hdr.fpV, 
%%      imagesc(volume(:,:,i)); 
%%      pause(0.05); 
%% end
%%
%% Copyright Ultrasonix Medical Corporation Nov 2009
%% Author: Reza Zahiri Azar
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [volume, hdr]  = loadvol(datapath, volN)

if nargin == 1
    volN = 1;
end

fid= fopen(datapath, 'r'); 
if( fid == -1), error('Cannot open file'); end

% read the header info
volinfo = fread(fid, 7, 'int');

hdr.datatype = volinfo(1); 
hdr.numVols = volinfo(2);
hdr.fpV = volinfo(3);
hdr.w = volinfo(4);
hdr.h = volinfo(5);
hdr.ss = volinfo(6);

% read the actual data
if(hdr.datatype == 0 ) % pre scanconversion B
    volume = zeros(hdr.h, hdr.w, hdr.fpV);
    for i = 1:hdr.fpV
        volume(:,:,i) = fread(fid, [hdr.h, hdr.w],'uint8'); 
    end
elseif (hdr.datatype == 1) % post scanconversion B
    volume = zeros(hdr.w, hdr.h, hdr.fpV);
    for i = 1:hdr.fpV
        volume(:,:,i) = fread(fid, [hdr.w, hdr.h],'uint8'); 
    end
elseif (hdr.datatype == 2) % rf data
    for i = 1:hdr.fpV
        [temp,count] = fread(fid, hdr.h*hdr.w,'int16'); 
        volume(:,:,i) = int16(reshape(temp, hdr.h, hdr.w));
    end
elseif (hdr.datatype == 3)  % velocity/variance   
    volume = zeros(hdr.w, hdr.h, hdr.fpV*2);
    for i = 1:hdr.fpV*2
        volume(:,:,i) = fread(fid, [hdr.w, hdr.h],'uint8'); 
    end
elseif (hdr.datatype == 4) % B+flow rgb    
    volume = zeros(hdr.w, hdr.h, hdr.fpV);
    for i = 1:hdr.fpV
        volume(:,:,i) = fread(fid, [hdr.w, hdr.h],'int32'); 
    end
end
    
fclose(fid);


