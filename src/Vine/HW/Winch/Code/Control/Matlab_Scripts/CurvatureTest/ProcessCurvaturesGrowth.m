clear; clc; close all;
directory = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/CurvatureTest/'; 
files = {'0_05_Growth.mat', '0_075_Growth.mat', '0_1_Growth.mat', '0_125_Growth.mat'};
pFiles = {'0_05.mat', '0_075.mat', '0_1.mat', '0_125.mat'};
movieFiles = {'0_05.mov','0_075.mov','0_1.mov','0_125.mov'};
endTimes = [183,120,82,114];

availableIndices = [1,2,3,4]
% colors = { [0,0.4470,0.7410],[0.6350,0.0780,0.1840],'k',[0.8500,0.3250,0.0980]};

ks = load(strcat(directory,'ks.mat'));
ks = ks.ks;

pressures = 15*([0.05; 0.075; 0.1; 0.125]-0.038);

curvatures = zeros(length(files),1);

figure(10);
hold on;

lineDist = @(p1,p2,x)(norm(cross(p2-p1,x-p1))/norm(p2-p1));

figure;
baseFun = @(k,p)(k(1)*p./(k(2)*p+1));
fun = @(k)(baseFun(k,pressures)-curvatures);

hs = [];

for i=1:length(files)
    figure;
    path = strcat(directory,files{i});
    
    points = load(path);
    points = points.points;
    
    pps = load(strcat(directory,pFiles{i}));
    pps = pps.points;
    
    lengthScale = norm(pps(1,:)-pps(2,:))
    lengthScale = 8.2e-2/lengthScale
    
    v = VideoReader(strcat(directory,movieFiles{i}));
    v.CurrentTime = endTimes(i);
    im = readFrame(v);
    imshow(im);
    
%     if i==4
%         p1 = points(1:end-2,:);
%         p2 = points(end-1:end,:);
%         [x y] = ginput(5);
%         points = vertcat(p1, [x y], p2);
%     end
    
    hold on;
    
    pts = [points zeros(size(points,1),1)];
    pts = pts(1:end-2,:);
    [r, c, circ] = findcurvature(pts, 100);
    
    scatter(points(:,1), points(:,2));
%     linePoints = points(end-1:end,:);
%     plot(linePoints(:,1),linePoints(:,2));
%     scatter(circ(:,1), circ(:,2));
    
    thetas = linspace(0,2*pi,1000).';
    ccs = repmat(c(1:2),size(thetas,1),1);
    circ = ccs+r*[cos(thetas) sin(thetas)];
%     scatter(circ(:,1),circ(:,2));
    
    deltas = repmat(points(1,:),size(thetas,1),1)-circ;
    [~,minIdx] = min(sum(deltas.^2,2));
    minTheta = thetas(minIdx);
    p1 = circ(minIdx,:);
    p2 = p1-1000*[-sin(minTheta) cos(minTheta)];
    plot([p1(1) p2(1)],[p1(2) p2(2)]);
    
    dataThetas = pts(:,1:2)-repmat(c(1:2),size(pts,1),1);
    dataThetas = atan2(dataThetas(:,2),dataThetas(:,1));
    projectedPoints = repmat(c(1:2),size(pts,1),1)+r*[cos(dataThetas) sin(dataThetas)];
    scatter(projectedPoints(:,1), projectedPoints(:,2),'r');
    
    rr = r;
    r = 1/baseFun(ks,pressures(i));
    
    figure(10);
    lengths = r*min(abs(dataThetas-minTheta),abs(2*pi-dataThetas-minTheta));
    dataDistances = lengthScale*cellfun(@(x)(lineDist([p1 0],[p2 0],x)),num2cell(pts,2));
    lengthsExpected = linspace(0,0.6,100);
    thetasExpected = lengthsExpected/r;
    expectedDistances = (r-r*cos(thetasExpected));
    
    if i<5
        ax = gca;
        ax.ColorOrderIndex = availableIndices(i);
        plot(lengthsExpected,expectedDistances,'LineWidth',4);
        ax.ColorOrderIndex = ax.ColorOrderIndex-1;
        scatter(lengths,dataDistances,150,'LineWidth',2);
        xlim([0 0.4]);
        ylim([0 0.25]);
        xlabel('Robot Length (m)');
        ylabel('Tip Deflection (m)');
    end
    
    rr = rr*lengthScale
    curvature = 1/rr
    curvatures(i) = curvature;
    
    x = 0;
end

h = figure(10);
set(h,'Position',[0 0 320 200]);
legendText = num2cell(num2str(kron(6.895*pressures,[1;1]),'%.2f kPa'),2);
for i=1:length(legendText)/2
    legendText{2*(i-1)+1} = strcat(legendText{2*(i-1)+1}, ' Model');
    legendText{2*(i-1)+2} = strcat(legendText{2*(i-1)+2}, ' Meas.');
end
legend(legendText);
xlabel('Robot Length (m)');
ylabel('Tip Deflection (m)');
set(gca,'FontSize',12,'FontName','Times New Roman');
box on;
grid on;
pause(1);
tightfig;


h = figure;
set(h,'Position',[0 0 320 200]);
ps = linspace(0,max(pressures)+0.1,100);
plot(ps*6.895,baseFun(ks,ps),'k','LineWidth',4);
hold on;

scatter(pressures*6.895, curvatures,300,'LineWidth',4);
% ylim([0 4]);
xlim([0 0.15]);

box on;
grid on;
xlim([0 max(ps)*6.895])
xlabel('sPAM Pressure (kPa)');
ylabel('Robot Curvature (1/m)');
legend('Model','Measured');
set(gca,'FontSize',12,'FontName','Times New Roman');
pause(1);
tightfig;
