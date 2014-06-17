
function show(res_re, res_ren, x, xaxis_label)


figure;
axes('FontSize', 14);

res_re = squeeze(res_re);
res_ren = squeeze(res_ren);


re = plot(x, res_re, 'rx-', 'LineWidth', 2, 'MarkerSize', 14);
hold on
ren = plot(x, res_ren, 'b*-', 'LineWidth', 2, 'MarkerSize', 11);
% plot(x, y(:,2), 'mo:', 'LineWidth', 2, 'MarkerSize', 9)
% plot(x, y(:,4), 'k+:', 'LineWidth', 2, 'MarkerSize', 8)
grid on

xlabel(xaxis_label, 'FontSize', 20)
% ylabel('Pose Recovery Error', 'FontSize', 20)
% title('Sensitivity of Pose Recovery to Fiducial Placement', 'FontSize', 20)
% legend([re,ren], 'Reconstruction Error (mm)', 'Relative Reconstruction Error (mm)');
text(25, 1.25, 'x Reconstruction Error (mm)    o Relative Reconstruction Error)', 'FontSize', 14, 'FontAngle', 'italic')
% text(25, 1.15, '* STD Translation (mm)      + STD Rotation (deg)', 'FontSize', 14, 'FontAngle', 'italic')


