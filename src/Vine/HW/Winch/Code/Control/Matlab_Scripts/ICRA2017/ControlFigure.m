clear; clc; close all;

addpath('../SysID');

fpath = '../SysID/Data/PointToPoint/Trial1.txt';

fid = fopen(fpath,'r');

errors = [];
props = [];
derivs = [];
integrals = [];
us = [];
dqs = [];
qs = [];

thetas = [];

tline = fgetl(fid);
while(ischar(tline))
    [A count] = sscanf(tline,'Actuator %d offset %f, %f');
    if(count == 0)
        break;
    end
    thetas(A(1)+1) = mod(atan2(A(3),A(2)),2*pi);
    tline = fgetl(fid);
end

while(ischar(tline))
    [Aerrors count] = sscanf(tline, 'error=[%f %f ];');
    if(count == 2)
        tline = fgetl(fid);
        count = 0;
        if(ischar(tline)) 
            [Aprop count] = sscanf(tline, ' proprotional=[%f %f ];');
        end
        if(count == 2)
            tline = fgetl(fid);
            count = 0;
            if(ischar(tline))
                [Aderiv count] = sscanf(tline, ' deriv=[%f %f ];');
            end
            if(count == 2)
                tline = fgetl(fid);
                count = 0;
                if(ischar(tline))
                    [Aint count] = sscanf(tline, ' integral=[%f %f ];');
                end
                if(count == 2)
                    tline = fgetl(fid);
                    count = 0;
                    if(ischar(tline))
                        [Au count] = sscanf(tline, ' u=[%f %f ];');
                    end
                    if(count == 2)
                        tline = fgetl(fid);
                        count = 0;
                        if(ischar(tline))
                            [Adqs count] = sscanf(tline, ' dqs= [%f %f %f ];');
                        end
                        if(count == 3)
                            tline = fgetl(fid);
                            count = 0;
                            if(ischar(tline))
                                [Aqs count] = sscanf(tline, ' qs= [%f %f %f ];');
                            end
                            if(count == 3)
                                errors = vertcat(errors, Aerrors.');
                                props = vertcat(props, Aprop.');
                                derivs = vertcat(derivs, Aderiv.');
                                integrals = vertcat(integrals, Aint.');
                                us = vertcat(us, Au.');
                                dqs = vertcat(dqs, Adqs.');
                                qs = vertcat(qs, Aqs.');
                            end
                        end
                    end
                end
            end
        end
    end
    tline = fgetl(fid);
end

fclose all;

h = figure;
subplot(2,1,1);
hold all;
% plot([1:size(errors,1)]/30,sqrt(sum(errors.^2,2)),'LineWidth',2);
plot([1:size(errors,1)]/30,errors(:,1),'LineWidth',4);
plot([1:size(errors,1)]/30,errors(:,2),'LineWidth',4);
box on; grid on;
ylabel('Error (Px)');
xlabel('Time (s)');
legend('x Error','y Error');
xlim([0 size(qs,1)/30]);
ylim([-200 50]);
set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');

subplot(2,1,2);
hold all;
plot([1:size(qs,1)]/30, 6.89*15*qs(:,1),'LineWidth',4);
plot([1:size(qs,1)]/30, 6.89*15*qs(:,2),'LineWidth',4);
plot([1:size(qs,1)]/30, 6.89*15*qs(:,3),'LineWidth',4);
xlim([0 size(qs,1)/30]);
xlabel('Time (s)');
ylabel('Pressure (kPa)');
legend('p_1','p_2','p_3','Orientation','horizontal');
ylim([0 15]);

set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');
set(h, 'Position', [0 0 320 300]);
box on;
grid on;
pause(1);
tightfig;
% title('errors');
% figure;
% hold all;
% plot([1:size(errors,1)],qs(:,1));
% plot([1:size(errors,1)],qs(:,2));
% plot([1:size(errors,1)],qs(:,3));
% title('qs');
% figure;
% hold all;
% plot([1:size(errors,1)],dqs(:,1));
% plot([1:size(errors,1)],dqs(:,2));
% plot([1:size(errors,1)],dqs(:,3));
% title('dqs');
% figure;
% hold all;
% plot([1:size(errors,1)],us(:,1));
% plot([1:size(errors,1)],us(:,2));
% title('us');
% figure;
% hold all;
% plot([1:size(errors,1)],props(:,1));
% plot([1:size(errors,1)],props(:,2));
% title('props');
% figure;
% hold all;
% plot([1:size(errors,1)],derivs(:,1));
% plot([1:size(errors,1)],derivs(:,2));
% title('derivs');
% figure;
% plot([1:size(errors,1)],sqrt(sum(derivs.^2,2)));
% title('derivsNorms');

if(false)
    figure;
    hold on;
    xlim([-200 200]);
    ylim([-200 200]);
    figHandles = [];
    hTitle = title('Frame');
    for i=1:size(errors,1)
        drawRobot(figHandles, zeros(2,1), thetas, qs(i,:).');
        if(~exist('quiverHandle'))
            quiverHandle = quiver(0, 0, errors(i,1),errors(i,2));
        end
        quiverHandle.UData = errors(i,1); quiverHandle.VData = errors(i,2);
        hTitle.String = sprintf('Frame %d of %d', i, size(errors,1));
        pause(1/30);
    end
end

% export_fig -transparent ControlDataFigure.pdf