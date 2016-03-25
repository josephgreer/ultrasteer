clear; clc; close all;

basePathIn = 'C:\Users\CHARM\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\2_29_16\';

snapPaths = rdir(strcat(basePathIn, 'Trial*\**\scan.png')); 
tileSize = 64;                  % tile size to extract around position
imageShift = 60;                % number of pixels to shift gps track up due to assumed SOS issues
wn = 0.1;                       % cut off frequency for high pass filter
pathSlash = '\';                % are we POSIX or win32
endPadding = 8;                 % chop off last endPadding datapoints
skip = 1;                       % skip data points

halfTileSize = tileSize/2;

ul = [111 85];
br = [550 530];
for i=1:length(snapPaths)
    currDir = snapPaths(i).name;
    currDir = strsplit(currDir,pathSlash);
    currDir = currDir(1:end-1);
    currDir = strcat(strjoin(currDir,pathSlash),pathSlash);
    outDir = strcat(currDir,'Tiles',pathSlash);
    
    pos = load(strcat(currDir, 'pos.mat'), '-ascii');
    force = load(strcat(currDir, 'force.mat'), '-ascii');
    
    
    % force magnitude
    magForce = sqrt(sum(force(:,1:3).^2, 2));

    
    % high pass filter the force signal
    if(length(magForce) < 3*10)
        display(sprintf('skipped i %d\n', i));
        continue;
    end
    [b,a] = butter(10,wn,'high');
    highPassForce = filtfilt(b,a,magForce);
    
    % velocity
    velocity = sqrt(sum((pos(2:end,4:end)-pos(1:end-1,4:end)).^2, 2));
    
    save(strcat(currDir,'highpass.mat'), '-ascii', 'highPassForce');
    save(strcat(currDir,'velocity.mat'), '-ascii', 'velocity');
    
    im = imread(strcat(currDir,'scan.png'));
    
    imwrite(im,strcat(currDir,'test.png'));
    
    npoints = size(pos,1)-endPadding;
    if(~exist(outDir, 'dir'))
        mkdir(outDir);
    end
    % if tile goes into darkness then skip this data point
    pos(:,2) = pos(:,2)-imageShift;
    ipos = round(pos);
    total = 0;
    if(true)
        for j=1:skip:npoints-endPadding
            if(ipos(j,1)-halfTileSize < ul(1) || ipos(j,2)-halfTileSize < ul(2)...
                    || ipos(j,1)+halfTileSize > br(1) || ipos(j,2)+halfTileSize > br(2))
                continue;
            end
            total = total+1;
            tile = im(ipos(j,2)-halfTileSize:ipos(j,2)+halfTileSize-1, ipos(j,1)-halfTileSize:ipos(j,1)+halfTileSize-1);
            
            %im(ipos(j,2)-halfTileSize:ipos(j,2)+halfTileSize-1, ipos(j,1)-halfTileSize:ipos(j,1)+halfTileSize-1) = 0;
            imwrite(tile, strcat(outDir, num2str(j), '.png'));
        end
    end
    display(sprintf('i %d total tiles %d', i, total));
end