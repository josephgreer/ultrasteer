function matched = resolve_loops(loops, all_cost, current_matched, pts_2d, trs, ex, ey, xo, yo, focal)

matched = [];

for i = 1:length(loops)
    combinations = sm_comb_all(loops{i});
    cost = sm_comb_all_cost2(combinations,pts_2d,trs,ex,ey,xo,yo,focal);
    k = size(loops{i}, 1);
    if k < 7
        if k <= 5
            [match match_cost] = [sm_combset_match(cost,k,combinations),zeros(k,1)];
        else
            match = [sm_combset_match_bypart(cost,k,5,combinations),zeros(k,1)];
        end
    else
        pts_2d_new = cell(length(pts_2d),1);
        for j = 1:3
            pts_2d_new{j} = pts_2d{j}(loops{i}(:,j),:);
        end
        
        for j = 4:length(pts_2d)
            pts_2d_new{j} = pts_2d{j};
        end
        
        hidden_seeds = cell(3,1);
        match = break_down_loops(pts_2d_new, pts_2d_new, trs, ex, ey, xo, yo, focal, size(loops{i}, 1), hidden_seeds);
%         img_tot = match_remaining(pts_2d_passed, trs, ex, ey, xo, yo, focal, seeds_tot);
%         match = [match zeros(size(match, 1),1)];
        
        for j = 1:3
            temp = loops{i}(:,j);
            match(:,j) = temp(match(:,j)); 
        end
    end
    matched = [matched; match];
    
    loops_all = [];
    for j = i + 1:length(loops)
        loops_all = [loops_all; loops{j}];
    end
    
    current_matched = [current_matched; match(:,1:3)];
    temp_cost = current_cost(all_cost, [current_matched(:,1:3); loops_all]);
end

matched = matched(:,1:3);