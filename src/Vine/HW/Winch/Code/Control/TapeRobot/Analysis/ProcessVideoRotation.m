clear; clc; close all;

v = VideoReader('/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/GrowingVine/HitTwoThings_Flat_Recompressed.mp4');
v.CurrentTime = 34;

% load GrowToLight1_Thetas.mat

thetas = [];

p1Features = [];
p1ValidPts = [];
frameLast = [];
frameIndex = 1;
while(hasFrame(v))
    frame = readFrame(v);
    if(frameIndex < length(thetas))
        display(sprintf('skipping frame %d', frameIndex));
        frameIndex = frameIndex+1;
        continue;
    end
    frame = uint8(mean(frame,3));
    p2 = detectSURFFeatures(frame);
    [p2Features,   p2ValidPts]  = extractFeatures(frame,  p2);
    if(~isempty(p1Features))
        indexPairs = matchFeatures(p1Features, p2Features);
        matchedP1 = p1ValidPts(indexPairs(:,1));
        matchedP2 = p2ValidPts(indexPairs(:,2));
        try
            [tform, inlierP2, inlierP1] = estimateGeometricTransform(...
                matchedP1, matchedP2, 'similarity');
            %         showMatchedFeatures(frameLast,frame, inlierP1, inlierP2);
            Tinv  = tform.invert.T;
            ss = Tinv(2,1);
            sc = Tinv(1,1);
            scale_recovered = sqrt(ss*ss + sc*sc);
            theta_recovered = atan2(ss,sc)*180/pi;
        catch
            theta_recovered = 0;
        end
        thetas = vertcat(thetas, theta_recovered);
    end
    
    frameLast = frame;
    p1Features = p2Features;
    p1ValidPts = p2ValidPts;
    display(frameIndex);
    frameIndex = frameIndex+1;
end

save('HitTwoThings_Flat_Recompressed', 'thetas');

% initAngle = -20;
% thetasTotal = cumsum(thetas);
% thetasTotal = thetasTotal+initAngle;
% d1 = designfilt('lowpassiir','FilterOrder',12, ...
%     'HalfPowerFrequency',0.01,'DesignMethod','butter');
% thetasTotalSmoothed = filtfilt(d1,double(thetasTotal));
% % plot(thetasTotal);
% % hold on;
% % plot(thetasTotalSmoothed);
% 
% vout = VideoWriter('/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/GrowingVine/GrowToLight1_Rotated.avi');
% vout.FrameRate = v.FrameRate;
% open(vout);
% 
% v.CurrentTime = 34;
% frameIndex = 0;
% while(hasFrame(v))
%     frame = readFrame(v);
%     if(frameIndex > 0)
%         frame = imrotate(frame, thetasTotal(min(frameIndex,length(thetasTotal))),'bilinear','crop');
%     else
%         frame = imrotate(frame, initAngle,'bilinear','crop');
%     end
%     writeVideo(vout, frame);
%     
%     display(frameIndex);
%     frameIndex = frameIndex+1;
% end
% close(vout);
% 
% 
% fclose all;