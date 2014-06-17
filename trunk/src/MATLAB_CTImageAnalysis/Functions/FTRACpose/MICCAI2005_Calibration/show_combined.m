
function show_combined(x1, res_re1, res_ren1, x2, res_re2, res_ren2, xaxis_label)

fig_handle = figure;
set(fig_handle, 'Position', [1 203 1400 948])
axes('FontSize', 14);

res_re2(1) = res_re1(end);
x2(1) = 50

plot_handle1 = subplot(1,2,1);
re = plot(x1, res_re1, 'rx-', 'LineWidth', 2, 'MarkerSize', 14);
hold on
ren = plot(x1, res_ren1, 'bo-', 'LineWidth', 2, 'MarkerSize', 11);
set(plot_handle1, 'XLim', [0 25]);
set(plot_handle1, 'YLim', [0 4]);
set(plot_handle1, 'FontSize', 30);
set(plot_handle1, 'XTick', [0 5 10 15 20]);
grid on
% set(plot_handle1, 'XTickLabel', [0 5 10 15 20]);
% legend_handle = legend([re,ren], 'Reconstruction Error (mm)', 'Relative Reconstruction Error (mm)', 'Location', 'NorthWest');
% set(legend_handle)

xlabel(xaxis_label, 'FontSize', 33)
ylabel('Seed Reconstruction Error (mm)', 'FontSize', 33)
text(1.5, 3.62, 'x Absolute', 'FontSize', 33, 'FontAngle', 'italic')
text(1.5, 3.38, '   Reconstruction', 'FontSize', 33, 'FontAngle', 'italic')
text(1.5, 2.85, 'o Relative', 'FontSize', 33, 'FontAngle', 'italic')
text(1.5, 2.6, '   Reconstruction', 'FontSize', 33, 'FontAngle', 'italic')

plot_handle2 = subplot(1,2,2);
plot(x2, res_re2, 'rx-', 'LineWidth', 2, 'MarkerSize', 14);
hold on
plot(x2, res_ren2, 'bo-', 'LineWidth', 2, 'MarkerSize', 11);

% set(plot_handle2, 'Axis', [40 520 0 4]);
% get(plot_handle2)
set(plot_handle2, 'YAxisLocation', 'right');
set(plot_handle2, 'YTickLabel', []);
set(plot_handle2, 'XTick', [50 150 250 350 450]);
set(plot_handle2, 'XLim', [50 450]);
set(plot_handle2, 'YLim', [0 4]);
set(plot_handle2, 'Position', [0.5703 - 0.1055 0.1100 0.3347 0.8150])
set(plot_handle2, 'FontSize', 30);
grid on


% plot(x, y(:,2), 'mo:', 'LineWidth', 2, 'MarkerSize', 9)
% plot(x, y(:,4), 'k+:', 'LineWidth', 2, 'MarkerSize', 8)


title('Sensitivity of Reconstruction to Focal Length Error', 'FontSize', 33)
% legend([re,ren], 'Reconstruction Error (mm)', 'Relative Reconstruction
% % Error (mm)');

% text(25, 1.15, '* STD Translation (mm)      + STD Rotation (deg)', 'FontSize', 14, 'FontAngle', 'italic')


