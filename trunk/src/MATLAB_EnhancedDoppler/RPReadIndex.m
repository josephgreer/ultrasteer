function [Im] = RPReadIndex(fid, header, frame)
if (frame > header.nframes)
    return
end

frame_count = 1;

% load the data and save into individual .mat files
if(header.filetype == 2^1) %.bpr
    Im = zeros(header.h, header.w, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.h, header.w],'uchar=>uchar'); 
    end
elseif(header.filetype == 2^2) %postscan B .b8
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w, header.h],'uint8'); 
    end
elseif(header.filetype == 2^3) %postscan B .b32
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w, header.h],'int32'); 
    end
elseif(header.filetype == 2^4) %rf 
    Im = zeros(header.h, header.w, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.h, header.w], 'int16'); 
    end
elseif(header.filetype == 2^5) %.mpr
    Im = zeros(header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,frame_count) = fread(fid,[header.h], 'uint8'); 
    end
elseif(header.filetype == 2^6) %.m
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w,header.h], 'uint8');
    end
elseif(header.filetype == 2^7) %.drf
    Im = zeros(header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,frame_count) = fread(fid, header.h, 'int16'); 
    end
elseif(header.filetype == 2^8) %.pw
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w,header.h], 'uint8');
    end
elseif(header.filetype == 2^9) %crf
    Im = zeros(header.h, header.w * header.extra);
    Im(:,:) = fread(fid,[header.h, header.w * header.extra],'int16'); 
elseif(header.filetype == 2^10) %.col
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid,[header.w, header.h],'int'); 
    end
elseif((header.filetype == 2^11)) %color .cvv 
    Im = zeros(header.w, header.h, numFrames, 2);
    for frame_count = 1:numFrames
        Im(:,:,frame_count, 1) = fread(fid,[header.w, header.h],'uint8'); 
        Im(:,:,frame_count, 2) = fread(fid,[header.w, header.h],'uint8'); 
    end
elseif(header.filetype == 2^12) %color vel
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w,header.h],'uchar=>uchar'); 
    end
elseif(header.filetype == 2^13) %.el
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w,header.h],'int32'); 
    end
elseif(header.filetype == 2^14) %.elo
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w,header.h],'uchar=>uchar'); 
    end
elseif(header.filetype == 2^15) %.epr
    Im = zeros(header.h, header.w, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.h,header.w],'uchar=>uchar'); 
    end
elseif(header.filetype == 2^16) %.ecg
    Im = zeros(header.w, header.h, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.w,header.h],'uchar=>uchar'); 
    end
elseif( header.filetype == 2^17 || header.filetype == 2^18 ) %.gps
    for frame_count = 1:numFrames
        gps_posx(:, frame_count) =  fread(fid, 1, 'double');   %8 bytes for double
        gps_posy(:, frame_count) = fread(fid, 1, 'double');
        gps_posz(:, frame_count) = fread(fid, 1, 'double');
        gps_a(:, frame_count) =  fread(fid, 1, 'double');  
        gps_e(:, frame_count) = fread(fid, 1, 'double');
        gps_r(:, frame_count) = fread(fid, 1, 'double');
        gps_s(:,:, frame_count) = fread(fid, 9, 'double');    %position matrix
        gps_q(:, frame_count) =  fread(fid, 4, 'double');   
        gps_time(:, frame_count) = fread(fid, 1, 'double');
        gps_quality(:, frame_count) = fread(fid, 1, 'ushort'); % 2 bytes for unsigned short
        Zeros(:, frame_count) =  fread(fid, 3, 'uint16');     % 6 bytes of padded zeroes
    end
    Im.gps_posx    = gps_posx;
    Im.gps_posy    = gps_posy;
    Im.gps_posz    = gps_posz;
    Im.gps_s       = gps_s;
    Im.gps_time    = gps_time;
    Im.gps_quality = gps_quality;
    Im.gps_Zeros   = Zeros;
    Im.gps_a    = gps_a;
    Im.gps_e    = gps_e;
    Im.gps_r    = gps_r;
    Im.gps_q    = gps_q;
elseif (header.filetype == 2^19) % time stamps
    Im = zeros(header.w, numFrames);
    for frame_count = 1:numFrames
        Im(:, frame_count) = fread(fid, header.w, 'uint32');
    end
elseif(header.filetype == 2^20) %.colmrf
    Im = zeros(header.h, header.extra, numFrames);
    for i=1:numFrames
        Im(:,:,i) = fread(fid, [header.h, header.extra], 'int16');
    end
elseif (header.filetype == 2^21) % .mrf
    Im = zeros(header.h, header.w, numFrames);
    for i=1:numFrames
        Im(:,:,i) = fread(fid, [header.h, header.w], 'int16');
    end
elseif(header.filetype == 2^22) % daqrf
    Im = zeros(header.h, header.w, numFrames);
    for frame_count = 1:numFrames
        Im(:,:,frame_count) = fread(fid, [header.h, header.w],'int16'); 
    end
elseif (header.filetype == 2^23) % 3dd volume (pre scan)
    Im = zeros(header.h, header.w, header.nframes);
    for i=1:header.nframes
        Im(:,:,i) = fread(fid, [header.h, header.w],'uchar=>uchar');
    end
elseif (header.filetype == 2^24) % 3d volume (post scan)
    Im = zeros(header.w, header.h, header.nframes);
    for i=1:header.nframes
        Im(:,:,i) = fread(fid, [header.w, header.h],'uchar=>uchar');
    end
else
    disp('Data not supported');
end

