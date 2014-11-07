% -------------------------------------------------------------------------
% author: Troy Adebar date: 2012-06-03
% title: Evaluate_and_Plot_MatlabIm
%
% summary: This .m file compares the Matlab 2D segmentations (the centroid 
% points within an image) of a saved color volume, 
% with the corresponding manual segmentation
% -------------------------------------------------------------------------
clear all; close all; clc;

cats ={'Curve','Lateral Position','Needle Diameter','Vibration Frequency'};
vals = {{'curved','straight'},{'center','lateral'},...
    {'380 microns','480 microns','580 microns'},{'low','med','high'}};
val_vec = [2 2 3 3];

error = [];
for cat = 1:4
    t_vals = vals{cat};
    for val = 1:val_vec(cat)
        for trial = 1:4
            % load data for the specific trial
            folder = [cats{cat} '/' t_vals{val} '/' num2str(trial) '/'];
            [mat_x,mat_y,mat_slice] = Segment_Needle(folder);
            load([folder 'manual_segmentation'])
            load([folder 'settings'])
            % compare at all slices for which we have data
            err = [];
            K = size(mat_slice,1);
            for k = 1:K
                slice = mat_slice(k);
                i = find(man_slices == slice);
                if( i )
                    err = [err; mat_x(k)-man_x_pix(i) mat_y(k)-man_y_pix(i)];
                end
            end
            
            % scale the points
            n = size(err,1);
            err = err.*micr_y./1000;
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
