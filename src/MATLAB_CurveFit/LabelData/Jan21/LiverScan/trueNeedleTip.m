needle = [290.214844 3.795117 26.789063]';

pos =  [267.444153 9.152929 49.336525]';
pose = [0.084351 0.996338 -0.004395; -0.017212 -0.002930 -0.999878; -0.996216 0.084473 0.016846];

imagePt = [219 255]';

calibration = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];
midPt = [320 0]';
imageMM = (imagePt-midPt)*83/1000;

sensor = calibration*[1 imageMM(2) imageMM(1)]';
world = (pose*sensor)+pos;


%%
I = imread('test2.bmp');
figure(1); clf;
imagesc(I);
%%
pos =  [274.364655 -1.116211 49.336525]';
pose = [0.141235 0.012573 0.989746; 0.088745 0.995605 -0.025391; -0.985962 0.091431 0.139526];
imagePt = [224 258]';

calibration = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];
midPt = [320 0]';
imageMM = (imagePt-midPt)*83/1000;

sensor = calibration*[1 imageMM(2) imageMM(1)]';
world = (pose*sensor)+pos;
%%
needle = [201.141211 41.299805 -179.709961]';
pos =  [226.144333 43.755470 -142.875000]';
pose = [0.127075 -0.991821 -0.008911; 0.010742 -0.007568 0.999878; -0.991821 -0.127319 0.009644];

imagePt = [522 238]';

calibration = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];
midPt = [320 0]';
imageMM = (imagePt-midPt)*83/1000;
imageMM(1) = imageMM(1);

sensor = calibration*[1 imageMM(2) imageMM(1)]';
world = (pose*sensor)+pos
