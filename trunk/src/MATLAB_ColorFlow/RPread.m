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
function [Im,header] = RPread(filename)

fid= fopen(filename, 'r');
fileExt = filename(end-3:end);

if( fid == -1)
    error('Cannot open file');
end

% read the header info
hinfo = fread(fid, 19, 'int32');

% load the header information into a structure and save under a separate file
header = struct('filetype', 0, 'nframes', 0, 'w', 0, 'h', 0, 'ss', 0, 'ul', [0,0], 'ur', [0,0], 'br', [0,0], 'bl', [0,0], 'probe',0, 'txf', 0, 'sf', 0, 'dr', 0, 'ld', 0, 'extra', 0);
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

v=[];
Im =[];
Im2=[];
% load the data and save into individual .mat files
 for frame_count = 1:header.nframes
 
     if(header.filetype == 2) %.bpr
        %Each frame has 4 byte header for frame number
        tag = fread(fid,1,'int32');
        [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
        Im(:,:,frame_count) = reshape(v,header.h,header.w);
   
    elseif(header.filetype == 4) %postscan B .b8
         tag = fread(fid,1,'int32');
         [v,count] = fread(fid,header.w*header.h,'int8'); 
         temp = int16(reshape(v,header.w,header.h));
         Im(:,:,frame_count) = imrotate(temp, -90); 
    
    elseif(header.filetype == 8) %postscan B .b32
         tag = fread(fid,1,'int32');
         [v,count] = fread(fid,header.w*header.h,'int32'); 
         temp = reshape(v,header.w,header.h);
         Im(:,:,frame_count) = imrotate(temp, -90); 
   
    elseif(header.filetype == 16) %rf
        tag = fread(fid,1,'int32');
        [v,count] = fread(fid,header.w*header.h,'int16'); 
        Im(:,:,frame_count) = int16(reshape(v,header.h,header.w));
        
    elseif(header.filetype == 32) %.mpr
        tag = fread(fid,1,'int32');
        [v,count] = fread(fid,header.w*header.h,'int16'); 
        Im(:,frame_count) = v;%int16(reshape(v,header.h,header.w));
    
    elseif(header.filetype == 64) %.m
        [v,count] = fread(fid,'uint8');
        temp = reshape(v,header.w,header.h);  
        Im(:,:,frame_count) = imrotate(temp,-90);
        
    elseif(header.filetype == 128) %.drf
        tag = fread(fid,1,'int32');
        [v,count] = fread(fid,header.h,'int16'); 
        Im(:,:,frame_count) = int16(reshape(v,header.w,header.h));
        
    elseif(header.filetype == 512) %crf
%         if(frame_count == 1)
%             tag = fread(fid,1,'int32');
%         end
        [v,count] = fread(fid,header.extra*header.w*header.h,'int16'); 
        Im(:,:,frame_count) = reshape(v,header.h,header.w*header.extra);
        %to obtain data per packet size use
        % Im(:,:,:,frame_count) = reshape(v,header.h,header.w,header.extra);
   
    elseif(header.filetype == 256) %.pw
        [v,count] = fread(fid,'uint8');
        temp = reshape(v,header.w,header.h);  
        Im(:,:,frame_count) = imrotate(temp,-90);
        
%     elseif(header.filetype == 1024) %.col        %The old file format for SONIX version 3.0X
%          [v,count] = fread(fid,header.w*header.h,'int'); 
%          temp = reshape(v,header.w,header.h);
%          temp2 = imrotate(temp, -90); 
%          Im(:,:,frame_count) = mirror(temp2,header.w);
%     
%     elseif((header.filetype == 2048) & (fileExt == '.sig')) %color .sig  %The old file format for SONIX version 3.0X
%         %Each frame has 4 byte header for frame number
%         tag = fread(fid,1,'int32'); 
%         [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
%         temp = reshape(v,header.w,header.h);   
%         temp2 = imrotate(temp, -90);
%         Im(:,:,frame_count) = mirror(temp2,header.w);
        
     elseif(header.filetype == 1024) %.col
         [v,count] = fread(fid,header.w*header.h,'int'); 
         temp = reshape(v,header.w,header.h);
         temp2 = imrotate(temp, -90); 
         Im(:,:,frame_count) = mirror(temp2,header.w);
        
    elseif((header.filetype == 2048)) %color .cvv (the new format as of SONIX version 3.1X)
        % velocity data
        [v,count] = fread(fid,header.w*header.h,'uint8'); 
        temp = reshape(v,header.w,header.h); 
        temp2 = imrotate(temp, -90);
        Im(:,:,frame_count) = mirror(temp2,header.w);
        
        % sigma
        [v,count] =fread(fid, header.w*header.h,'uint8');
        temp = reshape(v,header.w, header.h);
        temp2 = imrotate(temp, -90);
        Im2(:,:,frame_count) = mirror(temp2,header.w);
    
    elseif(header.filetype == 4096) %color vel
        %Each frame has 4 byte header for frame number
        tag = fread(fid,1,'int32'); 
        [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
        temp = reshape(v,header.w,header.h); 
        temp2 = imrotate(temp, -90);
        Im(:,:,frame_count) = mirror(temp2,header.w);
        
     elseif(header.filetype == 8192) %.el
        [v,count] = fread(fid,header.w*header.h,'int32'); 
        temp = reshape(v,header.w,header.h);
        temp2 = imrotate(temp, -90);
        Im(:,:,frame_count) = mirror(temp2,header.w);
    
    elseif(header.filetype == 16384) %.elo
        [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
        temp = int16(reshape(v,header.w,header.h));
        temp2 = imrotate(temp, -90); 
        Im(:,:,frame_count) = mirror(temp2,header.w);
  
    elseif(header.filetype == 32768) %.epr
        [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
        Im(:,:,frame_count) = int16(reshape(v,header.h,header.w));
       
    elseif(header.filetype == 65536) %.ecg
        [v,count] = fread(fid,header.w*header.h,'uchar=>uchar'); 
        Im = v;
    else
        disp('Data not supported');
    end
end
    
fclose(fid);


% % for RF data, plot both the RF center line and image 
% if(header.filetype == 16 || header.filetype == 512)
%     RPviewrf(Im, header, 24);
% 
% elseif(header.filetype == 128)
%     PDRFLine = Im(:,:,1);
%     figure, plot(PDRFLine, 'b');
%     title('PW RF');
%     
% elseif(header.filetype == 256)
%     figure, imagesc(Im(:,:,1));
%     colormap(gray);
%     title('PW spectrum');
% end
