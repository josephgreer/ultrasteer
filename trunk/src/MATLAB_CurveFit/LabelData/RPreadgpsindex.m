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

%%%%%%% Assumes 1-based index
function [res] = RPreadgpsindex(fid, header, index)

res = struct('pos', [0; 0; 0], 'posaer', [0; 0; 0], 'pose', [0 0 0; 0 0 0; 0 0 0], 'quality', 0);

if(fseek(fid, 76+168*(index-1), 'bof') == -1)
    error('unable to seek file');
end

 if (header.filetype == 131072 || header.filetype == 262144) %.gps
     gps_posx =  fread(fid, 1, 'double');   %8 bytes for double
     gps_posy =  fread(fid, 1, 'double');   %8 bytes for double
     gps_posz =  fread(fid, 1, 'double');   %8 bytes for double
     gps_posa =  fread(fid, 1, 'double');   %8 bytes for double         
     gps_pose =  fread(fid, 1, 'double');   %8 bytes for double    
     gps_posr =  fread(fid, 1, 'double');   %8 bytes for double      
     gps_s = fread(fid, 9, 'double');    %position matrix
     temp1 = fread(fid, 5, 'double'); %offset
     temp2 = fread(fid, 4, 'ushort'); % 2 bytes for unsigned short
     gps_quality = temp2(1);
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

res.pos = [gps_posx; gps_posy; gps_posz];
res.posaer = [gps_posa; gps_pose; gps_posr];
res.pose = reshape(gps_s, 3, 3);
res.quality = gps_quality;
end
