clear; clc; close all;

v = VideoReader('~/Dropbox (Stanford CHARM Lab)/Science Submission/GrowToLight_2_POV_Trimmed.mp4');
% v.CurrentTime = 34.112;

load thetas;

% p1Features = [];
% p1ValidPts = [];
% frameLast = [];
% frameIndex = 1;
% while(hasFrame(v))
%     frame = readFrame(v);
%     if(frameIndex < length(thetas))
%         display(sprintf('skipping frame %d', frameIndex));
%         frameIndex = frameIndex+1;
%         continue;
%     end
%     frame = uint8(mean(frame,3));
%     p2 = detectSURFFeatures(frame);
%     [p2Features,   p2ValidPts]  = extractFeatures(frame,  p2);
%     if(~isempty(p1Features))
%         indexPairs = matchFeatures(p1Features, p2Features);
%         matchedP1 = p1ValidPts(indexPairs(:,1));
%         matchedP2 = p2ValidPts(indexPairs(:,2));
%         [tform, inlierP2, inlierP1] = estimateGeometricTransform(...
%             matchedP1, matchedP2, 'similarity');
% %         showMatchedFeatures(frameLast,frame, inlierP1, inlierP2);
%         Tinv  = tform.invert.T;
%         ss = Tinv(2,1);
%         sc = Tinv(1,1);
%         scale_recovered = sqrt(ss*ss + sc*sc);
%         theta_recovered = atan2(ss,sc)*180/pi;
%         thetas = vertcat(thetas, theta_recovered);
%     end
%     
%     frameLast = frame;
%     p1Features = p2Features;
%     p1ValidPts = p2ValidPts;
%     display(frameIndex);
%     frameIndex = frameIndex+1;
% end

thetasTotal = cumsum(thetas);
d1 = designfilt('lowpassiir','FilterOrder',12, ...
    'HalfPowerFrequency',0.01,'DesignMethod','butter');
thetasTotalSmoothed = filtfilt(d1,double(thetasTotal));
% plot(thetasTotal);
% hold on;
% plot(thetasTotalSmoothed);

vout = VideoWriter('~/Dropbox (Stanford CHARM Lab)/Science Submission/GrowToLight_2_POV_Trimmed_Rotated.avi');
open(vout);

v.CurrentTime = 0;
frameIndex = 0;
while(hasFrame(v))
    frame = readFrame(v);
    if(frameIndex > 0)
        frame = imrotate(frame, thetasTotal(min(frameIndex,length(thetasTotal))),'bilinear','crop');
    end
    writeVideo(vout, frame);
    
    display(frameIndex);
    frameIndex = frameIndex+1;
end
close(vout);


fclose all;