clear; clc; close all;

calib = load('ActuationCalibration6.txt', '-ascii');

actuators = {calib(find(calib(:,1) == 0),:), calib(find(calib(:,1) == 1), :),...
    calib(find(calib(:,1) == 2), :)};

for i=1:length(actuators)
    actuators{i}(1,3:4) = 0;
    %actuators{i}(find(sqrt(sum(actuators{i}(:,3:4).^2,2)) < 0.25),3:4) = 0;
    actuators{i}(:,3) = cumsum(actuators{i}(:,3));
    actuators{i}(:,4) = cumsum(actuators{i}(:,4)); 
end

figure;
hold on;
colors = {'r', 'k', 'b'};
for i=1:length(actuators)
    scatter(actuators{i}(:,3), actuators{i}(:,4), colors{i});
end
daspect([1 1 1])

fsigm = @(p,xval) p(1)+(p(2)-p(1))./(1+10.^((p(3)-xval)*p(4)));

figure;
hold on;
for i=1:length(actuators)
    mag = sqrt(sum(actuators{i}(:,3:4).^2,2));
    mag = mag/max(mag);
    plot(actuators{i}(:,2),mag,colors{i});
    
    params = sigm_fit(actuators{i}(:,2), mag, [], [], 0);
    display(sprintf('actuator %d a=%f, b=%f, c=%f, d=%f',i,params(1),params(2),params(3),params(4)));
    plot(actuators{i}(:,2),fsigm(params,actuators{i}(:,2)),strcat(colors{mod(i+1,3)+1},'--'));
end


% syms a b c d x0 real
% simplify(fsigm([a,b,c,d], (d*c+log10(x0-a)-log10(b-x0))/d))
% 
% figure;
% hold on;
% for i=1:length(actuators)
%     mag = sqrt(sum(actuators{i}(:,3:4).^2,2));
%     mag = mag/max(mag);
%     plot(actuators{i}(:,2),mag,colors{i});
%     
%     params = sigm_fit(actuators{i}(:,2), mag, [], [], 0);
%     xs = [0:255];
%     mapped_xs = round(max(min((params(4)*params(3)+log10(max((xs/255)-params(1),1e-6))-log10(max(params(2)-(xs/255),1e-6)))/params(4), 255),0));
%     plot(xs,fsigm(params, mapped_xs),strcat(colors{mod(i+1,3)+1},'--'));
% end