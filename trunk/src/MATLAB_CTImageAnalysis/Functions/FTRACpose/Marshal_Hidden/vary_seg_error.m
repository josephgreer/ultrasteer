function vary_seg_error

profile on;

ns_im = [1 2 3 4;...
        1 2 3 5;...
        1 2 3 6;...
        1 2 4 5;...
        1 2 4 6;...
        1 2 5 6;...
        1 3 4 5;...
        1 3 4 6;...
        1 3 5 6;...
        1 4 5 6;...
        2 3 4 5;...
        2 3 4 6;...
        2 3 5 6;...
        2 4 5 6;...
        3 4 5 6];     
        
correct = 0;
incorrect = 0;
tot_hidden = 0;
res_re_avg = zeros(3,4); 




errors = [0 0.25 0.5 0.75 1 1.25 1.5 1.75 2];
percent_array = [];
res_re_array = cell(9,1);
for i = 1:9
    cmb_used = [ceil(rand(1,3)*15)];
    percent_all = [];
    for z = 1:2
        for j = 1:size(cmb_used, 2) 
            ns_im(cmb_used(j),:)
            [correct_count incorrect_count incorrect_hidden not_found total_hidden res_re res_ren] = match_seeds_hidden(ns_im(cmb_used(j),:), errors(i));     
            percent_all = [percent_all; correct_count / (correct_count + incorrect_count)];
            correct = correct + correct_count;
            incorrect = incorrect + incorrect_count;
            tot_hidden = tot_hidden + total_hidden;
            res_re_avg = res_re_avg + res_re;
        end
    end
    percent_all = sum(percent_all) / 6;
    res_re_avg = res_re_avg ./ 6;
    percent_array = [percent_array; percent_all];
    res_re_array{i} = res_re_avg; 
end

save 2_0__55_0__7_error percent_array res_re_array
%         save(strcat('Results\3_0__40_0__3\percent_', num2str(errors(i)), '.txt'), 'percent_all', '-ASCII');
%         save(strcat('Results\3_0__40_0__3\res_re_avg_', num2str(errors(i)), '.txt'), 'res_re_avg', '-ASCII');

profile report;
