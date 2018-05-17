clear; clc; close all;

imName = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/ObstaclePlanning/Data/VineRobotForest/Perspective_Corrected.jpg';

imshow(imresize(imread(imName),401/1820));
hold on;


scale = 400/2275;
o1 = [284.94342,570.14864 -18.03439,-36.06879 -9.61834,-34.86649 -3.60688,-24.04586 33.6642,-132.25222 44.48484,-137.0614 37.27108,-56.50776 31.25961,-48.09172 92.57656,-54.103183 144.27515,-9.618343 63.72152,3.606879 94.98114,68.530697 74.54216,138.26368 25.24816,135.85911 -6.01147,60.11464 -45.68713,80.55363 -69.73299,75.74445 -78.14904,64.92382 -167.11871,31.25961 -180.34394,-44.48483];
o2 = [1067.6361,539.49017 0,-46.28828 -18.0344,-48.69286 10.8207,-37.87223 33.6642,-30.65847 61.3169,-18.63554 37.2711,3.00573 40.2768,26.45045 52.9009,84.1605 6.0114,24.04586 -5.4103,19.23669 -30.6584,22.84356 -37.2711,9.61834 -14.4275,10.21949 -83.5594,-7.21375 -47.4906,-4.20803];
o3 = [1455.9767,654.30914 -12.0229,-61.31694 14.4275,-49.29401 44.4849,-43.28254 70.9352,-49.29401 51.6986,4.80917 81.7559,57.71006 28.8551,44.48484 14.4275,54.10318 -14.4275,32.46191 -90.172,66.12611 -67.3284,13.22522 -69.733,-18.03439];
o4 = [68.530697,760.11092 -31.259616,-50.4963 -3.606879,-43.28255 24.045858,-31.25961 75.74446,35.46764 25.84929,34.86649 -0.60114,51.6986 -30.65847,22.84356];
o5 = [178.53139,1246.3191 -40.80717,-45.908 0,-83.3147 32.30568,-54.4096 64.61136,-30.6053 108.71599,31.4127 40.91031,82.5073 -10.20179,76.5134 -93.01843,51.5925];
o6 = [835.07055,1513.7423 -179.6053,-148.8273 -67.16181,-110.9445 -13.17732,-107.5439 52.28419,-103.7183 129.22273,-70.98745 180.23169,-63.33614 150.90157,30.18031 119.446,61.63588 57.385,77.7887 22.9541,156.0024 -62.9111,156.0024 -84.5898,96.4919 -206.1613,32.7308];
o7 = [1404.4469,1034.6319 -51.0089,-45.90806 -30.6054,-64.61136 19.5534,-48.45852 40.8072,-22.95404 48.4585,16.15284 65.4615,56.96002 -0.8501,48.45852 -64.6114,61.2108];
o8 = [-3.4005979,1844.8244 -5.1008969,-151.3266 63.7612108,-163.2287 98.617344,-79.9141 79.0639,-24.6543 207.43647,-34.8562 73.963,19.5535 38.25673,48.4585 29.75523,196.3845 39.95703,157.2777 0.85015,47.6084 -34.00598,97.7671 -104.56839,64.6114 -90.11584,28.0549 -204.03588,-28.0549 -143.675259,-108.8191];
o9 = [1412.9484,1595.7306 -80.7642,-48.4586 -51.0089,-83.3146 3.4006,-62.9111 43.3576,-52.7092 111.3696,-44.2078 42.5074,-1.7003 45.058,31.4555 41.6573,68.012 -7.6514,92.6663 -50.1588,78.2137 -58.6603,27.2048];
o10 = [988.72384,2015.7044 -16.15284,-49.3087 -60.36061,-21.2537 -22.95404,-89.2657 20.40359,-79.914 52.70927,-49.3087 124.12179,-52.7093 42.5075,7.6514 44.2078,-17.8532 21.2537,23.8042 22.9541,90.966 6.8011,68.012 -71.4125,168.3296 -17.003,9.3516 -45.0579,-11.9021 -7.6514,44.2078 -46.7582,-17.8532];
o11 = [803.39126,2242.6943 -39.10688,-45.9081 5.1009,-56.96 52.70927,-60.3606 45.05791,3.4006 56.10987,58.6604 0.85015,62.911 -22.95403,31.4555 -51.85912,17.8532];
o12 = [1508.1652,2135.5755 -70.5624,-31.4555 -34.006,-74.8132 27.2048,-34.8561 42.5074,-26.3547 52.7093,11.052 78.2138,36.5564 32.3056,34.8561 5.9511,42.5075 -14.4526,28.0549 -72.2627,15.3027];

os = {o1,o2,o3,o4,o5,o6,o7,o8,o9,o10,o11,o12};

nodes = zeros(0,2);
map = zeros(0,4);
for i=1:length(os)
    o = os{i}*scale;
    currNodes = reshape(o,2,length(o)/2).';
    
    currNodes = cumsum(currNodes,1);
    currNodes = currNodes(end:-1:1,:);
    
    nodes = vertcat(nodes,currNodes);
    
    map = vertcat(map, [currNodes(1:end-1,:) currNodes(2:end,:)]);
    map = vertcat(map, [currNodes(end,:) currNodes(1,:)]);
    
    nodes = vertcat(nodes,currNodes(1,:));
    
%     plot(currNodes(:,1), currNodes(:,2),'LineWidth',5);
%     
%     for j=1:size(currNodes,1)
%         scatter(currNodes(j,1),currNodes(j,2),'LineWidth',3);
%         pause(0.25);
%     end
end

save('Maps/mapForest', 'map');
save('Maps/nodesForest', 'nodes');

DrawMap(map);
scatter(nodes(:,1),nodes(:,2));


