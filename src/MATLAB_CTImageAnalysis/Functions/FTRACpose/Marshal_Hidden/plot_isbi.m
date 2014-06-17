
load 2_0__55_0__1_error;

percent_array1 = percent_array;
res_re_array1 = res_re_array;


load 2_0__55_0__4_error;

percent_array2 = percent_array;
res_re_array2 = res_re_array;


load 2_0__55_0__7_error;

percent_array3 = percent_array;
res_re_array3 = res_re_array;

percent = percent_array1 + percent_array2 + percent_array3;
percent = percent ./ 3;

for i = 1:9
    res_re_array{i} = (res_re_array1{i} + res_re_array2{i} + res_re_array3{i}) ./ 3;
end

re = [];
for i = 1:9
    re = [re; res_re_array{i}(3,1)];
end 

percent = percent * 100;

fig_handle = figure;
% set(fig_handle, 'Position', [1 203 1400 948]);
ax1 = gca;

ax2 = axes('Position',get(ax1,'Position'),...
            'YAxisLocation','right',...
            'Color','none');
        

        
x = [0:0.25:2];

plot(ax1, x, percent, 'rx-', 'LineWidth', 3, 'MarkerSize', 16);
hold on;
plot(ax2, x, re, 'bo-', 'LineWidth', 3, 'MarkerSize', 13);

grid(ax1, 'on');
set(ax1, 'XTick', [0:0.25:2]);
set(ax1, 'XLim', [0 2]);
set(ax1, 'YTick', [0:10:100]);
set(ax1, 'YLim', [0 100]);
set(ax1, 'FontSize', 33);

set(ax2, 'XTick', []);
set(ax2, 'YTick', [0.5:0.5:5]);
set(ax2, 'YLim', [0 5]);
set(ax2, 'FontSize', 33);

xlabel(ax1, 'Segmentation Error (mm)', 'FontSize', 35)
ylabel(ax1, 'Matching Rate (%)', 'FontSize', 35)
ylabel(ax2, 'Mismatched Reconstruction Error (mm)', 'FontSize', 35)
title('Sensitivity to Segmentation Error', 'FontSize', 35);

text(0.25, 4, 'x Matching Rate', 'FontSize', 35, 'FontAngle', 'italic')
text(0.25, 3, 'o Mismatched Reconstruction', 'FontSize', 35, 'FontAngle', 'italic')
text(0.25, 2, '  Error', 'FontSize', 35, 'FontAngle', 'italic') 

load 2_0__55_0__1_percentage;

percent_array1 = percent_array;
res_re_array1 = res_re_array;


load 2_0__55_0__4_percentage;

percent_array2 = percent_array;
res_re_array2 = res_re_array;


load 2_0__55_0__7_percentage;

percent_array3 = percent_array;
res_re_array3 = res_re_array;

percent = percent_array1 + percent_array2 + percent_array3;
percent = percent ./ 3;

for i = 1:11
    res_re_array{i} = (res_re_array1{i} + res_re_array2{i} + res_re_array3{i}) ./ 3;
end
re = [];
for i = 1:11
    re = [re; res_re_array{i}(3,1)];
end

percent = percent * 100;

fig_handle = figure;
set(fig_handle, 'Position', [1 203 1400 948]);
ax1 = gca;
        
        
x = [0:2:20];

plot(ax1, x, percent, 'rx-', 'LineWidth', 3, 'MarkerSize', 16);

grid(ax1, 'on');

set(ax1, 'XTick', [0:2:20]);
set(ax1, 'XLim', [0 20]);
set(ax1, 'YTick', [0:10:100]);
set(ax1, 'YLim', [0 100]);
set(ax1, 'FontSize', 33);


xlabel(ax1, 'Hidden Seed Percentage (%)', 'FontSize', 35)
ylabel(ax1, 'Matching Rate (%)', 'FontSize', 35)
title('Sensitivity to Hidden Seed Percentage', 'FontSize', 35);