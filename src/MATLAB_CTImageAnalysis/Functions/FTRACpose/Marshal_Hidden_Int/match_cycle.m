function [f,n_c] = match_cycle(f, b, c, source)
% MATCH_CYCLE - perform the cycle canceling algorithm
%   This function calculates the min-cost flow via the cycle canceling
%   algorithm and returns the flow as f.  It also returns the number of
%   cycles required to find this flow as n_c.  

[n_node, m_node] = size(f);
mark = 1;
n_c = 0;
while mark == 1
	%calculate from f,b,c the residue graph;
	b_r = b - f;
    c_r = ones(n_node, m_node)*inf;
    [i, j] = find (c < inf);
    x = sub2ind(size(c), i, j);
    x_inv = sub2ind(size(c), j, i);
    c_r(x) = c(x);
    c_r(x_inv) = -c(x);

% 	for i = 1:n_node
%         for j = 1:n_node
%             if c(i,j)<inf
%                 c_r(i,j) = c(i,j);
%                 c_r(j,i) = -c(i,j);
%             end
%         end
% 	end
	%correct until now.
	%implement the Bellman-Ford method n-1 times;
	[mark,cyc] = match_bellmanford_matlab(b_r, c_r, source);
    n_c = n_c + 1;
	%correct until now.
	%update flow;
	if mark == 1
        n_cyc = length(cyc);

        % find minimum flow of cycle
        cyc_min = [];
        for i = 1:n_cyc
            k1 = cyc(i);
            if i < n_cyc
                k2 = cyc(i+1);
            else
                k2 = cyc(1);
            end
            cyc_min = [cyc_min b_r(k1,k2)];
        end
        
        f_c = min(cyc_min);
        
        for i = 1:n_cyc
            k1 = cyc(i);
            if i < n_cyc
                k2 = cyc(i+1);
            else
                k2 = cyc(1);
            end
            f(k1,k2) = f(k1,k2) + f_c;
            f(k2,k1) = f(k2,k1) - f_c;
        end        
%            cyc
%    for x = 1:size(cyc,2) - 1
%         c_r(cyc(1,x), cyc(1,x+1))
%    end
%    c_r(cyc(1,size(cyc,2)), cyc(1,1))
%    
%    flow_ind = find(f == 1);
%     total_cost = sum(c_r(flow_ind))
    end
end
n_c = n_c - 1;
%return to the top;
