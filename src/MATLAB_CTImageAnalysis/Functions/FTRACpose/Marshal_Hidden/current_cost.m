function matched_cost = current_cost(cost, matched)

x1 = sub2ind(size(cost{1}), matched(:,1), matched(:,2));
x2 = sub2ind(size(cost{2}), matched(:,2), matched(:,3));
% x3 = sub2ind(size(cost{3}), matched(:,3), matched(:,1));
matched_cost = sum(cost{1}(x1)) + sum(cost{2}(x2));% + sum(cost{3}(x3));