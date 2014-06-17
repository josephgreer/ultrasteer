function [mark,cyc] = match_bellmanford(b,c,source)

[n_node, m_node] = size(b);

%initialize single source;
d = ones(n_node,1)*100000;
pp = zeros(n_node,1);
d(1) = 0;

%Bellman-Ford;
for k = 1:n_node - 1
    for i = 1:n_node
        for j = 1:n_node
            % if ij is an edge in residue
            if b(i,j) == 1 || b(i,j) == 2
                %relax(i,j,w)
                if d(j) > d(i) + c(i,j)
                    d(j) = d(i) + c(i,j);
                    if d(j) > 90000
                        d(j) = 100000;
                    end
                    pp(j) = i;
                end
            end
        end
    end
end


for i = 1:n_node
    for j = 1:n_node
        % if ij is an edge in residue
        if b(i,j) == 1 || b(i,j) == 2
            %check negative cycle
            if d(j) > d(i) + c(i,j)
                k = 1;
                cyc = i;
                while 1 == 1
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
                return;
            end
        end
    end
end
cyc = 0;
mark = 0;
