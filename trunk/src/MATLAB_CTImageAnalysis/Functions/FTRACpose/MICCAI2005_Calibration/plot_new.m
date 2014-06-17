function plot_new

 % error = load('Ameet\final_err_1_25.mat');
% fin_err = squeeze(mean(error.fin_err,1))
% fin_err =  [mean([fin_err(1,:);fin_err(2,:)], 1);...
%             mean([fin_err(3,:);fin_err(4,:)], 1);...
%             mean([fin_err(5,:);fin_err(6,:)], 1)];
%         
% error2 = load('Ameet\final_err_20_500.mat');
% fin_err2 = squeeze(mean(error2.fin_err,1))
% fin_err2 =  [mean([fin_err2(1,:);fin_err2(2,:)], 1);...
%             mean([fin_err2(3,:);fin_err2(4,:)], 1);...
%             mean([fin_err2(5,:);fin_err2(6,:)], 1)];            
%         
% 
% x1 = [1:25];
% x2 = [40:20:500];
% 
% fin_err2 = fin_err2(:,2:end);
% fig_handle = figure;
% set(fig_handle, 'Position', [1 203 1400 948])
% axes('FontSize', 14);
% 
% fin_err2(:,1) = fin_err(:,end);
% x2(1) = 50;
% 
% plot_handle1 = subplot(1,2,1);
% re = plot(x1, fin_err(1,:), 'rx-', 'LineWidth', 2, 'MarkerSize', 14);
% hold on
% ren = plot(x1, fin_err(2,:), 'bo-', 'LineWidth', 2, 'MarkerSize', 11);
% ren = plot(x1, fin_err(3,:), 'k.-', 'LineWidth', 2, 'MarkerSize', 11);
% set(plot_handle1, 'XLim', [0 25]);
% set(plot_handle1, 'YLim', [0 40]);
% set(plot_handle1, 'FontSize', 30);
% set(plot_handle1, 'XTick', [0 5 10 15 20]);
% grid on
% 
% xlabel('Error in Focal Spot (mm)', 'FontSize', 33)
% ylabel('Error in Prediction (mm or deg)', 'FontSize', 33)
% text(3, 36, 'x Translation (mm)', 'FontSize', 33, 'FontAngle', 'italic')
% text(3, 32, 'o Amount of', 'FontSize', 33, 'FontAngle', 'italic')
% text(3, 30, '   Rotation (deg)', 'FontSize', 33, 'FontAngle', 'italic')
% text(3, 26, '   Axis of', 'FontSize', 33, 'FontAngle', 'italic')
% text(3, 24, '   Rotation (deg)', 'FontSize', 33, 'FontAngle', 'italic')
% text(3, 27.25, '.', 'FontSize', 60, 'FontAngle', 'italic')
% 
% 
% plot_handle2 = subplot(1,2,2);
% re = plot(x2, fin_err2(1,:), 'rx-', 'LineWidth', 2, 'MarkerSize', 14);
% hold on
% ren = plot(x2, fin_err2(2,:), 'bo-', 'LineWidth', 2, 'MarkerSize', 11);
% ren = plot(x2, fin_err2(3,:), 'k.-', 'LineWidth', 2, 'MarkerSize', 11);
% 
% set(plot_handle2, 'YAxisLocation', 'right');
% set(plot_handle2, 'YTickLabel', []);
% set(plot_handle2, 'XTick', [50 150 250 350 450]);
% set(plot_handle2, 'XLim', [50 450]);
% set(plot_handle2, 'YLim', [0 40]);
% set(plot_handle2, 'Position', [0.5703 - 0.1055 0.1100 0.3347 0.8150])
% set(plot_handle2, 'FontSize', 30);
% grid on
% 
% title('Validity of the Affine Model', 'FontSize', 33)




error = load('Ameet\relative_err_1_2_20.mat');
err_small = squeeze(mean(error.err_small,1));
translation_small = sqrt(err_small(1,:).^2+err_small(2,:).^2+err_small(3,:).^2)
rotation_small = sqrt(err_small(4,:).^2+err_small(5,:).^2+err_small(6,:).^2)

        
error2 = load('Ameet\relative_err_1_40_520.mat');
err_large = squeeze(mean(error2.err_large,1));
translation_large = sqrt(err_large(1,:).^2+err_large(2,:).^2+err_large(3,:).^2)
rotation_large = sqrt(err_large(4,:).^2+err_large(5,:).^2+err_large(6,:).^2)


x1 = [2:2:20];
x2 = [40:20:520];

err_large = err_large(:,2:end);
fig_handle = figure;
set(fig_handle, 'Position', [1 203 1400 948])
axes('FontSize', 14);

err_large(:,1) = err_small(:,end);
x2(1) = 50;

plot_handle1 = subplot(1,2,1);
re = plot(x1, translation_small, 'rx-', 'LineWidth', 2, 'MarkerSize', 14);
hold on
ren = plot(x1, rotation_small, 'bo-', 'LineWidth', 2, 'MarkerSize', 11);
set(plot_handle1, 'XLim', [0 20]);
set(plot_handle1, 'YLim', [0.2 1.2]);
set(plot_handle1, 'FontSize', 30);
set(plot_handle1, 'XTick', [0 5 10 15]);
grid on

xlabel('Error in Focal Spot (mm)', 'FontSize', 33)
ylabel('Error in Relative Pose (mm or deg)', 'FontSize', 33)
text(3, 1.05, 'x Translation (mm)', 'FontSize', 33, 'FontAngle', 'italic')
text(3, 0.95, 'o Rotation (deg)', 'FontSize', 33, 'FontAngle', 'italic')


plot_handle2 = subplot(1,2,2);
re = plot(x2, translation_large, 'rx-', 'LineWidth', 2, 'MarkerSize', 14);
hold on
ren = plot(x2, rotation_large, 'bo-', 'LineWidth', 2, 'MarkerSize', 11);

set(plot_handle2, 'YAxisLocation', 'right');
set(plot_handle2, 'YTickLabel', []);
set(plot_handle2, 'XTick', [50 150 250 350 450]);
set(plot_handle2, 'XLim', [50 450]);
set(plot_handle2, 'YLim', [.2 1.2]);
set(plot_handle2, 'Position', [0.5703 - 0.1055 0.1100 0.3347 0.8150])
set(plot_handle2, 'FontSize', 30);
grid on

title('Sensitivity of Relative C-Arm Pose Estimation to Calibration', 'FontSize', 33)