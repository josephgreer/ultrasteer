% -------------------------------------------------------------------------
% author: Troy Adebar date: 2012-06-03
% title: Evaluate_and_Plot_ITKpoly
%
% summary: This .m file compares the ITK segmentations of a number of
% needles with manual segmentations
% -------------------------------------------------------------------------
clear all; close all; clc;

cats ={'Needle Configuration','Lateral Position','Needle Diameter','Vibration Frequency'};
vals = {{'Curved','Straight'},{'Center','Lateral'},...
    {'380 microns','480 microns','580 microns'},{'Low','Med','High'}};
val_vec = [2 2 3 3];

error = [];
for cat = 1:4
    t_vals = vals{cat};
    for val = 1:val_vec(cat)
        for trial = 1:4
            % load data for the specific trial
            folder = [cats{cat} '/' t_vals{val} '/' num2str(trial) '/'];
            [a,b] = readPolyFile(folder);
            load([folder 'manual_segmentation'])
            load([folder 'settings'])
            % scan convert and scale the manual points
            man_pts = scanconvertpoints([man_x_pix',man_y_pix'],...
                man_slices',degPerFr,fpV,probeR3D_mm,...
                micr_x,micr_y,o_x,o_y,m,n);
            % Construct points from the polynomial fit
            z = man_pts(:,3);
            x = polyval(a,z); y = polyval(b,z);
            itk_pts = [x,y,z];
            % Evaluate and save the errors
            n = size(itk_pts,1);
            err = itk_pts(:,1:2)-man_pts(:,1:2);
            err_3D = sqrt(sum(err.^2,2));
            error = [error; err_3D err...
                cat*ones(n,1) val*ones(n,1) trial*ones(n,1)];
            % Tabulate error by trial as we will discard the worst
            ind = (error(:,4) == cat) & (error(:,5) == val) & (error(:,6) == trial);
            err_by_trial(trial) = mean(error(ind,1));
        end
        display(err_by_trial);
        % After all four trials have been scanned, discard the worst
        bad_trial = find(err_by_trial == max(err_by_trial));
        ind = (error(:,4) == cat) & (error(:,5) == val) & (error(:,6) == bad_trial);
        error(ind,:) = [];
    end
    
    error = [ error];
    this_cat_error = error(error(:,4) == cat,[1 5]);
    
    fontsize = 13;
    hFig = figure(cat);
    h1 = boxplot(this_cat_error(:,1),this_cat_error(:,2),'labels',t_vals);
    set(h1(7,:),'Visible','off')
    set(gca, 'FontSize', fontsize)
    xlabel(cats{cat});
    ylabel('2D Localization Error (mm)');
    ylim([0 10])
    set(gca,'YTick',1:10)
    set(gcf,'Color','w')
    h = findobj(gca,'Type','text');
    set(h,'FontSize',fontsize)
    xlabh = get(gca,'XLabel');
    set(xlabh,'Position',get(xlabh,'Position') - [0 5 0])
    export_fig('PRF.pdf')
end
