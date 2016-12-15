clear; clc; close all;

figure; hold on;

tube = 1;

ks = [];

files = rdir(sprintf('Data/Stiffness/%d_*.txt', tube));

pressures = [];

for i=1:length(files)
    fname = files(i).name;
    figure(1); hold on;
    
    pressure = sscanf(fname, 'Data/Stiffness/%d_%d.%d.txt');
    pressure = pressure(2)/10.0+pressure(3)/100.0;
    
    data = load(fname);
    
    data(:,2) = data(:,2)-data(1,2);
    data(:,2) = 4.4452016*data(:,2);
    data(:,3) = 4.4452016*data(:,3);
    hold on;
    plot(data(:,1), data(:,2))
    ax = gca;
    ax.ColorOrderIndex = ax.ColorOrderIndex-1;
    plot(data(:,1), data(:,3));
    
    pressures = [pressures pressure];
    
    A = [data(:,1) ones(size(data,1),1)];
    b = data(:,2);
    k = A\b
    ks = [ks; k(1)];
    
%     degreeFit = 1;
%     A = vander(amnts);
%     A = A(:,end-degreeFit:end);
%     b = ks;
%     mb = A\b
    
%     ys = polyval(mb, amnts);
%     plot(amnts, ys, 'r--');
  % close 1;
end
hold on;
figure(2); hold on;
plot(pressures, ks);
