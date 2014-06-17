function [mark,cyc] = match_bellmanford_matlab(b, c, source)

[n_node, m_node] = size(b);

%initialize single source;
d = ones(n_node,1)*inf;
pp = zeros(n_node,1);
d(source) = 0;

[i, j] = find(b > 0);
x = sub2ind(size(b), i, j);

% length(i)


% pp1 = pp;
% % 
% pp = zeros(n_node,1);
% % c(x)

% VERIFY NEGATIVE COST CYCLE FORMATION
% VERIFY SUPPOSED ZERO COST CYCLES
for m = 1:n_node - 1
    for n = 1:length(i)
        %relax(i,j,w)
        if d(j(n)) > d(i(n)) + c(i(n),j(n)) + 0.000001
            d(j(n)) = d(i(n)) + c(i(n),j(n));
%             if d(j(n)) > 90000
%                 d(j(n)) = 100000;
%             end
            pp(j(n)) = i(n);
        end         
    end
    if (mod(m, 10) == 0)
        d_c = d(i) + c(x);
        n_ind = find(d(j) > d_c + 0.000001);
        if (~isempty(n_ind))
%             if (n_node < 50)
%                1 
%             end
            try
                k = 1;
                cyc = i(n_ind(1));
                while 1 == 1
            %         cyc
                    i_cyc = find(cyc == pp(cyc(k)));
                    if ~isempty(i_cyc)
                        n_cyc = length(cyc);
                        cyc = cyc(i_cyc:n_cyc);
                        break;
                    else
                        cyc = [cyc,pp(cyc(k))];
                        k = k + 1;
                    end
                end
                cyc = fliplr(cyc);
                mark = 1;
%                 cyc
%                 m
                return;
            catch
                
            end
        end
    end
end

cyc = 0;
mark = 0;

% d1 = d;
% pp1 = pp;
% inf_cap = find(b == 0);
% c_new = c;
% c_new(inf_cap) = 100000;
% d = ones(n_node,1) * 100000;
% d(source) = 0;
% pp = zeros(n_node,1);

% for m = 1:n_node - 1
%     d_rep = repmat(d, 1, n_node);
%     y = c_new + d_rep;
%     
%     [d_new, j] = min(y, [], 1);
%     
%     d_comp = [d d_new'];
%     [d_updated, updated] = min(d_comp, [], 2);
%     updated = updated - 1;
%     pp = updated .* j' + (1 - updated) .* pp;
%     d = updated .* d_new' + (1 - updated) .* d;
% end

% pp = pp';
% d = d'

% x = 0;
% y = 0;
% x = find(b > 0);
% [i, j] = find(b > 0);
% d_c = d(i) + c(x);
% n_ind = find(d(j) > d_c);


