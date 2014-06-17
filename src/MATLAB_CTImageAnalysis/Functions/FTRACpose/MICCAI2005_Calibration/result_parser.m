function [averages, minmax] = result_parser

% focal error 0 - 25
averages = zeros(4,4,26);
minmax = zeros(4,2,26);

x = [0:25];

for i = x
    [a b c d] = textread(strcat('Results\Focal_Length_Err-', int2str(i), '.txt'), '%f %f %f %f', 4);
    averages(:,:,i+1) = [a b c d];

    [a b] = textread(strcat('Results\Focal_Length_Err-', int2str(i), '.txt'), '%f %f', 'headerlines', 4);
    minmax(:,:,i+1) = [a b];
end

% show(averages(3,1,:), averages(4,1,:), x, 'Focal Length Error (mm)');

% focal error 0 - 500
averages2 = zeros(4,4,26);
minmax2 = zeros(4,2,26);

x2 = [0:20:500];

for i = 1:26
    [a b c d] = textread(strcat('Results\Focal_Length_Err-', int2str(x2(i)), '.txt'), '%f %f %f %f', 4);
    averages2(:,:,i) = [a b c d];

    [a b] = textread(strcat('Results\Focal_Length_Err-', int2str(x2(i)), '.txt'), '%f %f', 'headerlines', 4);
    minmax2(:,:,i) = [a b];
end

show_combined(x,squeeze(averages(3,1,:)),squeeze(averages(4,1,:)),x2(3:end),squeeze(averages2(3,1,3:end)),squeeze(averages2(4,1,3:end)),'Focal Length Error (mm)');

% % 
% % origin error 0 - 25
% % averages = zeros(4,4,26);
% % minmax = zeros(4,2,26);
% % 
% % x = [0:25];
% % 
% % for i = x
% %     [a b c d] = textread(strcat('Results\Origin_Err-', int2str(i), '.txt'), '%f %f %f %f', 4);
% %     averages(:,:,i+1) = [a b c d];
% % 
% %     [a b] = textread(strcat('Results\Origin_Err-', int2str(i), '.txt'), '%f %f', 'headerlines', 4);
% %     minmax(:,:,i+1) = [a b];
% % end
% % 
% % show(averages(3,1,:), averages(4,1,:), x, 'Origin Error (pixels)');
% % 
% % origin error 0 - 200
% % averages2 = zeros(4,4,11);
% % minmax2 = zeros(4,2,11);
% % x2 = [0:20:200];
% % 
% % for i = 1:11
% %     [a b c d] = textread(strcat('Results\Origin_Err-', int2str(x2(i)), '.txt'), '%f %f %f %f', 4);
% %     averages2(:,:,i) = [a b c d];
% % 
% %     [a b] = textread(strcat('Results\Origin_Err-', int2str(x2(i)), '.txt'), '%f %f', 'headerlines', 4);
% %     minmax2(:,:,i) = [a b];
% % end
% % 
% % figure;
% % plot([x,x2(3:end)], [squeeze(averages(3,1,:));squeeze(averages2(3,1,3:end))]);
% % hold on
% % plot([x,x2(3:end)], [squeeze(averages(4,1,:));squeeze(averages2(4,1,3:end))]);
% % show_combined_origin(x,squeeze(averages(3,1,:)),squeeze(averages(4,1,:)),x2(3:end),squeeze(averages2(3,1,3:end)),squeeze(averages2(4,1,3:end)),'Origin Error (mm)');
% % show(averages2(3,1,:), averages2(4,1,:), x2, 'Origin Error (pixels)');