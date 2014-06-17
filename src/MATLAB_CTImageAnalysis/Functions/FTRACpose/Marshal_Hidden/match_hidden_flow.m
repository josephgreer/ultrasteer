function f = match_hidden_flow(n_seeds1, n_seeds2, total_seeds)


% f = zeros(n_seeds1 + 2, n_seeds2 + 2);
% 
% f(2:1 + n_seeds1) = 1;
% f(n_seeds1 + 2, 2:1 + n_seeds2) = 1;
% 
% for i = 1:n_seeds1
%     f(i + 1, i + 1) = 1;
% end
%         

f = zeros(n_seeds1 + n_seeds2 + 2);

hidden_seeds1 = total_seeds - n_seeds1;
hidden_seeds2 = total_seeds - n_seeds2;

if (n_seeds1 <= n_seeds2)
    for i = 1:hidden_seeds1
        % source to nodes
        f(1, i + 1) = 1;
        f(i + 1, 1) = -1;

        % nodes to nodes
        f(1 + n_seeds1 + i, i + 1) = -1;
        f(i + 1, 1 + n_seeds1 + i) = 1;

        % nodes to sink
        f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = 1;
        f(n_seeds1 + n_seeds2 + 2, 1 + n_seeds1 + i) = -1;
    end
    
%     % hidden seeds in image 1
%     for i = 1:hidden_seeds1
%         % source to nodes
%         f(1, i + 1) = f(1, i + 1) + 1;
%         f(i + 1, 1) = f(i + 1, 1) - 1;
%         
%         % nodes to nodes
%         f(i + 1, 2 + n_seeds1 + i) = 1;
%         f(2 + n_seeds1 + i, i + 1) = -1;
%         
%         % nodes to sink
%         f(2 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = f(2 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) + 1;
%         f(n_seeds1 + n_seeds2 + 2, 2 + n_seeds1 + i) = f(n_seeds1 + n_seeds2 + 2, 2 + n_seeds1 + i) - 1;
%     end
    
elseif (n_seeds1 > n_seeds2)
    for i = 1:n_seeds1
        % source to nodes
        f(1, i + 1) = 1;
        f(i + 1, 1) = -1;

        % nodes to sink
        f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = 1;
        f(n_seeds1 + n_seeds2 + 2, 1 + n_seeds1 + i) = -1;
    end
    
    for i = 1:n_seeds2
        % nodes to nodes
        f(1 + n_seeds1 + i, i + 1) = -1;
        f(i + 1, 1 + n_seeds1 + i) = 1;
    end
    
    for i = 1:hidden_seeds2
        
    end
end

% f=zeros(n_seeds1*2+2, n_seeds2*2+2);

% f = zeros(n_seeds1 + n_seeds2 + 2);
% 
% for i = 1:n_seeds1
%     f(i + 1,1) = -1;
%     f(i + 1, 1 + n_seeds2 + i) = 1;
%     f(n_seeds1 + n_seeds2 + 2, 1 + n_seeds2 + i) = -1;
% end
% for i = 1:n_seeds1
%     f(1 ,i + 1) = 1;
%     f(1 + n_seeds1 + i,i + 1) = -1;
%     f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = 1;
% end