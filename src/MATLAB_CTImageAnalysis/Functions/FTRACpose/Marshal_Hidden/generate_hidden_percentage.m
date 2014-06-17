function [pts_2d_hidden, hidden_seeds] = generate_hidden_percentage(pts_2d, percentage)
% GENERATE_HIDDEN_PERCENTAGE - generates data with hidden seeds using the
% percentage provided as input.  The data will have this given percentage
% of the seeds hidden.

number_hidden = round(percentage * length(pts_2d));

% test = pts_2d;
n_seeds = length(pts_2d);

closest_seed = [];
	
for i = 1:length(pts_2d)
    current_point = pts_2d(i,:);
    current_point = repmat(current_point, length(pts_2d), 1);

    % distance between seeds
    distance = abs(current_point - pts_2d);
    distance = sqrt(distance(:,1).^2 + distance(:,2).^2);
%         distance1 = [distance1 distance];
    distance = [[1:n_seeds]' distance];
    
    % sort and pick min
    distance = sortrows(distance, 2);
    closest_seed = [closest_seed; i distance(2, 1) distance(2, 2)];
        
end

% closest_seed = [[1:length(closest_seed)]' closest_seed];
closest_seed = sortrows(closest_seed, 3);

distance1 = [];
hidden_seeds = [];

% for i = 1:number_hidden
%     
%     % 'hide' seed
% %     pts_2d(closest_seed(2*i - 1,1),:) = [0 0];
%     hidden_seeds = [hidden_seeds; closest_seed(2*i - 1,1) closest_seed(2*i,1)];
%     
% end

i = 1;
j = 1;
while (i ~= number_hidden + 1);
    if (isempty(find(ismember(closest_seed(j,:), hidden_seeds) == 1)))
        pts_2d(closest_seed(j,1),:) = [0 0];
        hidden_seeds = [hidden_seeds; closest_seed(j, 1) closest_seed(j, 2)];
        i = i + 1;
    end
    j = j + 1;
end

if (~isempty(hidden_seeds))
    hidden_seeds = sortrows(hidden_seeds, 1);
end

hidden_seeds

[x_ind, y_ind, pts_2d] = find(pts_2d);
pts_2d = reshape(pts_2d, length(pts_2d) / 2, 2);

% size(pts_2d)
pts_2d_hidden = pts_2d;