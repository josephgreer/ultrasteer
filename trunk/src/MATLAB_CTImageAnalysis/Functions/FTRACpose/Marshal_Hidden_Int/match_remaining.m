function matched_seeds = match_remaining(pts_2d, pts_2d_hidden, cost, node_edges, flow, trs, ex, ey, xo, yo, focal, matching, hidden_seeds, total_seeds)
                 
% seed numbers
n_seeds = cell(4,1);
for i = 1:3
    n_seeds{i} = size(pts_2d{i}, 1);
end
n_seeds{4} = n_seeds{1};

% hidden_seeds = cell(3,1);
image_hidden = cell(4,1);
for i = 1:4
    temp = matching(:,i);
    temp = sort(temp);
    prev = 0;
    for j = 1:length(temp)
        if (temp(j) == prev)
            image_hidden{i} = [image_hidden{i} temp(j)];
        end
        prev = temp(j);
    end
end
total_seeds
n_seeds

[matched_seeds, loops, mis_matched_seeds, bpe_thresh] = find_loops(matching, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds);
[matched_seeds, loops, mis_matched_seeds] = break_links(pts_2d, pts_2d_hidden, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matching, hidden_seeds, total_seeds)
1


[matched_seeds, total_matching, bad_loops, bpe_sum] = process_image_matching(matched_seeds, loops, mis_matched_seeds, bpe_thresh, cost, pts_2d, trs, ex, ey, xo, yo, focal);

for i = 1:size(matched_seeds, 1)
    cost{1}(matched_seeds(i,1), matched_seeds(i,2)) = 0;
    cost{2}(matched_seeds(i,2), matched_seeds(i,3)) = 0;
    cost{3}(matched_seeds(i,3), matched_seeds(i,1)) = 0;
end

bpe_all = calculate_bpe(pts_2d, mis_matched_seeds, trs, ex, ey, xo, yo, focal);

match_ind = find(mis_matched_seeds(:,1) == mis_matched_seeds(:,4) & bpe_all < bpe_thresh);
mis_ind = find(mis_matched_seeds(:,1) ~= mis_matched_seeds(:,4) | bpe_all >= bpe_thresh);
% high_ind = find(bpe_all >= bpe_thresh);

mis_matched = [mis_matched_seeds(mis_ind,1:4) bpe_all(mis_ind)];

bpe_all_old = bpe_all;
bpe_min = inf;
matched_min = [];
mis_matched_min = [];
bad_loops_min = [];
total_matching_min = total_matching;
cost_ind = [];
mis_ind_old = zeros(1,100);

x = 0;
while(~isempty(mis_ind))
    for i = 1:size(mis_matched, 1)
        link = mis_matched(i,:)

        for j = 1:3
            j
            cost_old = cost{j}(link(j),link(j + 1));
            cost{j}(link(j),link(j + 1)) = 1000;
            [matched, total_matching, mis_matched_seeds, bad_loops, bpe_sum] = run_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, mis_matched_seeds, hidden_seeds, total_seeds);
            if (bpe_sum < bpe_min)
                bpe_min = bpe_sum
                matched_min = matched;
                mis_matched_min = mis_matched_seeds;
                bad_loops_min = bad_loops;
                total_matching_min = total_matching;
                cost_ind = [i j];
            else
                cost{j}(link(j),link(j + 1)) = cost_old;
            end
        end
    end
    bpe_min = inf;
    link = mis_matched(cost_ind(1),:);
    mis_matched_seeds = mis_matched_min;
    bpe_all = calculate_bpe(pts_2d, mis_matched_seeds, trs, ex, ey, xo, yo, focal);

    mis_ind_old = mis_ind;
    match_ind = find(mis_matched_seeds(:,1) == mis_matched_seeds(:,4) & bpe_all < bpe_thresh);
    mis_ind = find(mis_matched_seeds(:,1) ~= mis_matched_seeds(:,4) | bpe_all >= bpe_thresh);
%     high_ind = find(bpe_all >= bpe_thresh);

    matched = matched_min;
    mis_matched = [mis_matched_seeds(mis_ind,1:4) bpe_all(mis_ind)];
    
    j = cost_ind(2);
    cost{j}(link(j),link(j + 1)) = 1000;
    x = x + 1;
    if (x == 10)
        break;
    end;
end

matched_seeds = total_matching_min;
1

% [matched, bad_loops, bpe_sum] = run_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, mis_matched_seeds, hidden_seeds, total_seeds);
% i = 1;

% while (~isempty(mis_matched_seeds))
%     link = mis_matched_seeds(i,:)
% %     ind
%     
%     cost_old = cost{1}(link(1),link(2));
%     cost{1}(link(1),link(2)) = 1000;
%     
%     [matched1, mis_matched1, total_d_ra1, ind1] = run_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, mis_matched_seeds, hidden_seeds, total_seeds);
%     cost1 = cost{1};
%     cost{1}(link(1),link(2)) = cost_old;
%     
%     
%     cost_old = cost{2}(link(2),link(3));
%     cost{2}(link(2),link(3)) = 1000;
%     
%     [matched2, mis_matched2, total_d_ra2, ind2] = run_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, mis_matched_seeds, hidden_seeds, total_seeds);
%     cost2 = cost{2};
%     cost{2}(link(2),link(3)) = cost_old;
%     
%     
%     cost_old = cost{3}(link(3),link(4));
%     cost{3}(link(3),link(4)) = 1000;
%     
%     [matched3, mis_matched3, total_d_ra3, ind3] = run_network(pts_2d, pts_2d_hidden, n_seeds, cost, node_edges, flow, bpe_thresh, trs, ex, ey, xo, yo, focal, matched_seeds, mis_matched_seeds, hidden_seeds, total_seeds);
%     cost3 = cost{3};
%     cost{3}(link(3),link(1)) = cost_old;
%     
%     [min_total, min_ind] = min([total_d_ra1 total_d_ra2 total_d_ra3]);
%     
%     if (min_total + 0.00001 < total_d_ra)
%         min_total
%         total_d_ra
%         if (min_ind == 1)
%             ind = ind1;
%             cost{1} = cost1;
%             matched_seeds = matched1;
%             mis_matched_seeds = mis_matched1;   
% %             loops = loops1;
%         elseif (min_ind == 2)        
%             ind = ind2;
%             cost{2} = cost2;
%             matched_seeds = matched2;
%             mis_matched_seeds = mis_matched2;     
% %             loops = loops2;
%         elseif (min_ind == 3)        
%             ind = ind3;
%             cost{3} = cost3;
%             matched_seeds = matched3;
%             mis_matched_seeds = mis_matched3;     
% %             loops = loops3;
%         end
% %         all_seeds = [matched_seeds; mis_matched_seeds];
% %         [matched_sub, loops_sub, mis_matched_seeds, max_d] = find_loops(mis_matched_seeds, cost, pts_2d, pts_2d_hidden, trs, ex, ey, xo, yo, focal, hidden_seeds, total_seeds)
% %         matched_seeds = [matched_seeds; matched_sub];
% %         all_seeds = [matched_seeds; mis_matched_seeds];
% %         total_d_ra_old = total_d_ra;
%         total_d_ra = min_total;
%         
%     else
%         i = i + 1;
%         if (i > size(mis_matched_seeds, 1))
%             mis_matched = mis_matched_seeds;
%             mis_matched_seeds = [];
%         end
% %         last = size(ind,1);
% %         if (last ~= 0)
% %             ind = ind(2:last);
% %         else
% %             ind = [];
% %         end
%     end
% end