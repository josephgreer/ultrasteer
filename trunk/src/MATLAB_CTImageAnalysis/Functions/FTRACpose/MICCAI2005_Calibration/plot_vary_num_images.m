function plot_vary_num_images

error0 = load('Results\Total_Err-0-Varying.mat');
% error10 = load('Results\Total_Err-10.mat');
error20 = load('Results\Total_Err-20.mat');
error100 = load('Results\Total_Err-100.mat');

img_nums = [2:6];
res_re_0 = [];
res_re_10 = [];
res_re_20 = [];
res_re_100 = [];

for i = 1:5
    res_re_0 = [res_re_0, error0.averages(3,1,i)];
%     res_re_10 = [res_re_10, error10.averages(3,1,i)];
    res_re_20 = [res_re_20, error20.averages(3,1,i)];
    res_re_100 = [res_re_100, error100.averages(3,1,i)];
end

fig_handle = figure;
set(fig_handle, 'Position', [1 203 1400 948]);

plot(img_nums, res_re_0,  'rx-', 'LineWidth', 2, 'MarkerSize', 14);
hold on;
% plot(img_nums, res_re_10);
plot(img_nums, res_re_20, 'k+-', 'LineWidth', 2, 'MarkerSize', 11);
plot(img_nums, res_re_100, 'bo-', 'LineWidth', 2, 'MarkerSize', 11);

grid on;

set(gca, 'XTick', [2 3 4 5 6]);
set(gca, 'XLim', [2 6]);
set(gca, 'FontSize', 30);

text(4.6, 1.15, 'x No error', 'FontSize', 33, 'FontAngle', 'italic')
text(4.6, 1.05, '+ 20 mm error', 'FontSize', 33, 'FontAngle', 'italic')
text(4.6, 0.95, 'o 100 mm error', 'FontSize', 33, 'FontAngle', 'italic')


xlabel('Number of Images', 'FontSize', 33)
ylabel('Absolute Reconstruction Error (mm)', 'FontSize', 33)
title('Sensitivity of Reconstruction to the Number of Images', 'FontSize', 33);

