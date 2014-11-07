% -------------------------------------------------------------------------
% author: Troy Adebar date: 2012-06-03
% title: Evaluate_and_Plot_Matlabpoly
%
% summary: This .m file compares the Matlab 3D segmentation (polynomial fit
% to Matlab image points) of a saved color volume, 
% with the corresponding manual segmentation
% -------------------------------------------------------------------------
%% Initialize
clear all; close all; clc;
%dbstop in Segment_Needle at 30;

cats ={'Needle Configuration','Lateral Position','Needle Diameter','Vibration Frequency'};
vals = {{'Curved','Straight'},{'Center','Lateral'},...
    {'380 microns','480 microns','580 microns'},{'Low','Med','High'}};

all_vals = {{'Curved','Straight'},{'Center','Lateral'},...
    {'380 microns','480 microns','580 microns'},{'400 Hz','600 Hz','800 Hz'}};
val_vec = [2 2 3 3];
filenames = {'bend','lateral','diam','vibfreq'};

%% Process data
error = [];
for cat = 1:1
    t_vals = vals{cat};
    for val = 1:1%val_vec(cat)
        for trial = 1:1
            % load data for the specific trial
            folder = 'V:\NeedleScan\9_26_14_MagnetScan\Scan 4\';%[cats{cat} '/' t_vals{val} '/' num2str(trial) '/']
            [mat_x,mat_y,mat_slice] = Segment_Needle(folder);
           % load([folder 'manual_segmentation'])
            load([folder 'settings'])
            % scan convert manual points
%             man_pts = scanconvertpoints([man_x_pix',man_y_pix'],...
%                 man_slices',degPerFr,fpV,probeR3D_mm,...
%                 micr_x,micr_y,o_x,o_y,m,n);
            % scan convert Matlab points
            matlab_pts = scanconvertpoints([mat_x,mat_y],...
                mat_slice,degPerFr,fpV,probeR3D_mm,...
                micr_x,micr_y,o_x,o_y,m,n);
            % fit polynomial curves to the Matlab points
            px = polyfit(matlab_pts(:,3),matlab_pts(:,1),3);
            py = polyfit(matlab_pts(:,3),matlab_pts(:,2),3);
            
            min_z = min(matlab_pts(:,3));  max_z = max(matlab_pts(:,3));
            zs = [min_z:(max_z-min_z)/100:max_z]';
            fit_pts = [polyval(px,zs) polyval(py,zs) zs];
            
            plot3(fit_pts(:,1),fit_pts(:,2),fit_pts(:,3));
            hold on;
            scatter3(matlab_pts(:,1),matlab_pts(:,2), matlab_pts(:,3));
            axis equal;
        end
    end
end
% Save the data to make plotting faster
save('Error','error');

%% Plot the results in individual plots
% Load the data
load('Error');
for cat = 1:4
    % Extract the errors for the current category from the big matrix
    this_cat_error = error(error(:,4) == cat,[1 5]);
    % Create a boxplot with the extracted results
    fontsize = 13;
    hFig = figure(cat);
    t_vals = all_vals{cat};
    h1 = boxplot(this_cat_error(:,1),this_cat_error(:,2),'labels',t_vals);
    set(h1(7,:),'Visible','off')
    set(gca, 'FontSize', fontsize,'FontName','Times New Roman');
    xlabel(cats{cat});
    ylabel('Localization Error (mm)');
    y_lims = {4,4,4,4};
    ylim([0 y_lims{cat}])
    set(gca,'YTick',0:1:y_lims{cat})
    set(gcf,'Color','w')
    h = findobj(gca,'Type','text');
    set(h,'FontSize',fontsize,'FontName','Times New Roman')
    xlabh = get(gca,'XLabel');
%     set(xlabh,'Position',get(xlabh,'Position') - [0 5 0])
    export_fig([filenames{cat} '.pdf'])
end

%% Plot one box plot with all results
load('Error');
all_vals = {'Curved','Straight',[];
           'Center','Lateral',[];
           '380 microns','480 microns','580 microns';
           '400 Hz','600 Hz','800 Hz'};

N = size(error,1);
for i = 1:N
    val_labels(i,1) =  all_vals(error(i,4),error(i,5));
end
       
fontsize = 13;
hFig = figure(5);
h1 = boxplot(error(:,1),{error(:,4) error(:,5)},'labels',{cats(error(:,4))' val_labels},...
    'notch','on','factorseparator',[1],'labelverbosity','minor');
set(h1(7,:),'Visible','off')
set(gca, 'FontSize', fontsize,'FontName','Times New Roman');
ylabel('Localization Error (mm)');
maxY = 4;
ylim([0 maxY])
set(gca,'YTick',0:1:maxY)
set(gcf,'Color','w')
h = findobj(gca,'Type','text');
set(h,'FontSize',fontsize,'FontName','Times New Roman')
set(hFig,'units','normalized','outerposition',[0 0 1 0.75]);
xlabh = get(gca,'XLabel');
% set(xlabh,'Position',get(xlabh,'Position') - [0 5 0])
export_fig(['SegmentationAccuracy.pdf'])
