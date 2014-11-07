%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% loads the ultrasound RF data saved from the Sonix software
%%
%% Inputs:  
%%     filename - The path of the data to open
%%
%% Return:
%%     Im -         The image data returned into a 3D array (h, w, numframes)
%%     header -     The file header information   
%%
%% Corina Leung, corina.leung@ultrasonix.com
%% Ultrasonix Medical Corporation Nov 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Im] = RPreadframeindex(fid, header, index)

Im =[];
v = [];

if(header.filetype ~= 4)
    error('wrong file type');
end
    
if(fseek(fid, 76+(index-1)*header.w*header.h, 'bof') == -1)
    error('could not seek file'); 
end

 if(header.filetype == 2) %.bpr
    %Each frame has 4 byte header for frame number
    %tag = fread(fid,1,'int32');
    [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
    Im = reshape(v,header.h,header.w);

elseif(header.filetype == 4) %postscan B .b8
     %tag = fread(fid,1,'int32');
     [v,count] = fread(fid,header.w*header.h,'uint8'); 
     temp = reshape(v,header.w,header.h);
     Im = transpose(temp);
     %Im = imrotate(temp, -90); 

elseif(header.filetype == 8) %postscan B .b32
     %tag = fread(fid,1,'int32');
     [v,count] = fread(fid,header.w*header.h,'int8'); 
     temp = reshape(v,header.w,header.h);
     Im = imrotate(temp, -90); 

elseif(header.filetype == 16) %rf
    %tag = fread(fid,1,'int32');
    [v,count] = fread(fid,header.w*header.h,'unit8'); 
    Im = int16(reshape(v,header.h,header.w));

elseif(header.filetype == 32) %.mpr
    %tag = fread(fid,1,'int32');
    [v,count] = fread(fid,header.w*header.h,'int16'); 
    Im(:) = v;%int16(reshape(v,header.h,header.w));

elseif(header.filetype == 64) %.m
    [v,count] = fread(fid,'uint8');
    temp = reshape(v,header.w,header.h);  
    Im = imrotate(temp,-90);

elseif(header.filetype == 128) %.drf
    %tag = fread(fid,1,'int32');
    [v,count] = fread(fid,header.h,'int16'); 
    Im = int16(reshape(v,header.w,header.h));

elseif(header.filetype == 512) %crf
    %tag = fread(fid,1,'int32');
    [v,count] = fread(fid,header.extra*header.w*header.h,'int16'); 
    Im = reshape(v,header.h,header.w*header.extra);
    %to obtain data per packet size use
    % Im(:,:,:) = reshape(v,header.h,header.w,header.extra);

elseif(header.filetype == 256) %.pw
    [v,count] = fread(fid,'uint8');
    temp = reshape(v,header.w,header.h);  
    Im = imrotate(temp,-90);

%     elseif(header.filetype == 1024) %.col        %The old file format for SONIX version 3.0X
%          [v,count] = fread(fid,header.w*header.h,'int'); 
%          temp = reshape(v,header.w,header.h);
%          temp2 = imrotate(temp, -90); 
%          Im = mirror(temp2,header.w);
%     
%     elseif((header.filetype == 2048) & (fileExt == '.sig')) %color .sig  %The old file format for SONIX version 3.0X
%         %Each frame has 4 byte header for frame number
%         tag = fread(fid,1,'int32'); 
%         [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
%         temp = reshape(v,header.w,header.h);   
%         temp2 = imrotate(temp, -90);
%         Im = mirror(temp2,header.w);

 elseif(header.filetype == 1024) %.col
     [v,count] = fread(fid,header.w*header.h*4,'uint8'); 
     temp = reshape(v,header.w,header.h,4);
     temp2 = imrotate(temp, -90); 
     Im = mirror(temp2,header.w);

elseif((header.filetype == 2048)) %color .cvv (the new format as of SONIX version 3.1X)
    % velocity data
    [v,count] = fread(fid,header.w*header.h,'uint8'); 
    temp = reshape(v,header.w,header.h); 
    temp2 = imrotate(temp, -90);
    Im = mirror(temp2,header.w);

    % sigma
    [v,count] =fread(fid, header.w*header.h,'uint8');
    temp = reshape(v,header.w, header.h);
    temp2 = imrotate(temp, -90);
    Im2(:,:) = mirror(temp2,header.w);

elseif(header.filetype == 4096) %color vel
    %Each frame has 4 byte header for frame number
    %tag = fread(fid,1,'int32'); 
    [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
    temp = reshape(v,header.w,header.h); 
    temp2 = imrotate(temp, -90);
    Im = mirror(temp2,header.w);

 elseif(header.filetype == 8192) %.el
    [v,count] = fread(fid,header.w*header.h,'int32'); 
    temp = reshape(v,header.w,header.h);
    temp2 = imrotate(temp, -90);
    Im = mirror(temp2,header.w);

elseif(header.filetype == 16384) %.elo
    [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
    temp = int16(reshape(v,header.w,header.h));
    temp2 = imrotate(temp, -90); 
    Im = mirror(temp2,header.w);

elseif(header.filetype == 32768) %.epr
    [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
    Im = int16(reshape(v,header.h,header.w));

elseif(header.filetype == 65536) %.ecg
    [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
    Im = v;

 elseif (header.filetype == 131072) %.gps
     gps_posx(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double
     gps_posy(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double
     gps_posz(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double
     gps_posa(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double         
     gps_pose(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double    
     gps_posr(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double      
     gps_s(:,:, frame_count) = fread(fid, 9, 'double');    %position matrix
     temp1 = fread(fid, 5, 'double'); %offset
     temp2 = fread(fid, 4, 'ushort'); % 2 bytes for unsigned short
     gps_quality(:, frame_count) = temp2(1);
%          gps_posx(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double
%          gps_posy(:, frame_count) = fread(fid, 1, 'double');
%          gps_posz(:, frame_count) = fread(fid, 1, 'double');
%          gps_s(:,:, frame_count) = fread(fid, 9, 'double');    %position matrix
%          gps_time(:, frame_count) = fread(fid, 1, 'double');
%          gps_quality(:, frame_count) = fread(fid, 1, 'ushort'); % 2 bytes for unsigned short
%          zeros(:, frame_count) =  fread(fid, 3, 'uint16');     % 6 bytes of padded zeroes
 else
    disp('Data not supported');
 end
