function f = create_flow(n_seeds1, n_seeds2, total_seeds)

f = zeros(n_seeds1 + n_seeds2 + 2);

% % FIX FOR EQUAL NUMBER OF HIDDEN SEEDS
% 
% for i = 1:total_seeds
%     % source to nodes
%     f(1, mod(i - 1, n_seeds1) + 2) = f(1, mod(i - 1, n_seeds1) + 2) + 1;
%     
%     % nodes to nodes
%     f(mod(i - 1, n_seeds1) + 2, mod(i - 1, n_seeds2) + n_seeds1 + 2) = 1;
%     f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds1) + 2) = -1;
%     
%     % nodes to sink
%     f(mod(i - 1, n_seeds2) + n_seeds1 + 2, n_seeds1 + n_seeds2 + 2)  = f(mod(i - 1, n_seeds2) + n_seeds1 + 2, n_seeds1 + n_seeds2 + 2) + 1;
%     
% end

% more hidden seeds in image 1
if (n_seeds1 <= n_seeds2)
    hidden_seeds1 = total_seeds - n_seeds1;
    
    for i = 1:n_seeds1
        % source to nodes
        f(1, i + 1) = 1;
    %         f(i + 1, 1) = -1;

        % nodes to nodes
        f(1 + n_seeds1 + i, i + 1) = -1;
        f(i + 1, 1 + n_seeds1 + i) = 1;

        % nodes to sink
        f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = 1;
    %         f(n_seeds1 + n_seeds2 + 2, 1 + n_seeds1 + i) = -1;
    end
% 
    for i = 1:hidden_seeds1
       % source to nodes
       f(1, i + 1) = f(1, i + 1) + 1;

       % nodes to nodes
       f(i + 1, n_seeds1 + n_seeds2 + 2 - i) = 1;
       f(n_seeds1 + n_seeds2 + 2 - i, i + 1) = -1;

       % nodes to sink
       f(n_seeds1 + n_seeds2 + 2 - i, n_seeds1 + n_seeds2 + 2) = f(n_seeds1 + n_seeds2 + 2 - i, n_seeds1 + n_seeds2 + 2) + 1;
    end
    
% more hidden seeds in image 2
else
    hidden_seeds2 = total_seeds - n_seeds2;
    
    for i = 1:n_seeds2
        % source to nodes
        f(1, i + 1) = 1;
    %         f(i + 1, 1) = -1;

        % nodes to nodes
        f(1 + n_seeds1 + i, i + 1) = -1;
        f(i + 1, 1 + n_seeds1 + i) = 1;

        % nodes to sink
        f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = 1;
    %         f(n_seeds1 + n_seeds2 + 2, 1 + n_seeds1 + i) = -1;
    end
    
    for i = 1:hidden_seeds2
       % source to nodes
       f(1, 2 + n_seeds1 - i) = f(1, 2 + n_seeds1 - i) + 1;
%        f(1, i + 1) = f(1, i + 1) + 1;

       % nodes to nodes
       f(2 + n_seeds1 - i, 2 + n_seeds1 + n_seeds2 - i) = 1;
       f(2 + n_seeds1 + n_seeds2 - i, 2 + n_seeds1 - i) = -1;
%        f(i + 1, n_seeds1 + n_seeds2 + 2 - i) = 1;
%        f(n_seeds1 + n_seeds2 + 2 - i, i + 1) = -1;

       % nodes to sink
       f(2 + n_seeds1 + n_seeds2 - i, 2 + n_seeds1 + n_seeds2) = f(2 + n_seeds1 + n_seeds2 - i, 2 + n_seeds1 + n_seeds2) + 1;
%        f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) = f(1 + n_seeds1 + i, n_seeds1 + n_seeds2 + 2) + 1;
       
%        f(n_seeds1 + n_seeds2 + 2 - i, n_seeds1 + n_seeds2 + 2) = f(n_seeds1 + n_seeds2 + 2 - i, n_seeds1 + n_seeds2 + 2) + 1;
    end
end