clear; clc; close all;
directory = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/CurvatureTest/'; 
files = {'0_05.mat', '0_075.mat', '0_1.mat', '0_125b.mat'};

pressures = 15*([0.05; 0.075; 0.1; 0.125]-0.038);

curvatures = zeros(length(files),1);

for i=1:length(files)
    path = strcat(directory,files{i});
    
    points = load(path);
    points = points.points;
    
    pts = [points zeros(size(points,1),1)];
    pts = pts(3:end,:);
    [r, c, circ] = findcurvature(pts, 100);
    
    lengthScale = norm(points(1,:)-points(2,:))
    lengthScale = 8.2e-2/lengthScale
    
    r = r*lengthScale
    curvature = 1/r
    curvatures(i) = curvature;
end


baseFun = @(k,p)(k(1)*p./(k(2)*p+1));
fun = @(k)(baseFun(k,pressures)-curvatures);

ks = lsqnonlin(fun, [1; 1]);

ps = linspace(0,max(pressures)+0.1,100);
plot(ps,baseFun(ks,ps),'k','LineWidth',4);
hold on;

scatter(pressures, curvatures,300,'LineWidth',4);
ylim([0 4]);
xlim([0 0.15]);

box on;
grid on;
xlim([0 max(ps)])
xlabel('sPAM Pressure (PSI)');
ylabel('Curvature (1/m)');
