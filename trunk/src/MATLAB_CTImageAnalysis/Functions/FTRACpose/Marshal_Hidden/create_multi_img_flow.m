function f = create_multi_img_flow(n_seeds1, n_seeds2, n_seeds3, total_seeds)

f = zeros(2 * n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + 2);
[n_row, n_col] = size(f);

for i = 1:total_seeds
    % source to nodes
    f(1, mod(i - 1, n_seeds1) + 2) = f(1, mod(i - 1, n_seeds1) + 2) + 1;
    
    % image 1 to image 2
    f(mod(i - 1, n_seeds1) + 2, mod(i - 1, n_seeds2) + n_seeds1 + 2) = f(mod(i - 1, n_seeds1) + 2, mod(i - 1, n_seeds2) + n_seeds1 + 2) + 1;
    f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds1) + 2) = -1;        

    % intermmediary nodes 2 - 2
    f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2) = f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2) + 1;

    % image 2 to image 3
    f(mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2, mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + 2) = 1;
    f(mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + 2, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2) = -1;
    
    % intermmediary nodes 3 - 3
    f(mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + 2, mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + n_seeds3 + 2) = f(mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + 2, mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + n_seeds3 + 2) + 1;
    
    % image 3 to image 1
    f(mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + n_seeds3 + 2, mod(i - 1, n_seeds1) + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + 2) = 1;
    f(mod(i - 1, n_seeds1) + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + 2, mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + n_seeds3 + 2) = -1;

    % nodes to sink
    f(mod(i - 1, n_seeds1) + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + 2, n_col) = f(mod(i - 1, n_seeds1) + n_seeds1 + 2 * n_seeds2 + 2 * n_seeds3 + 2, n_col) + 1;
    
%     if (n_seeds1 == n_seeds2 && n_seeds1 < i)
%         % equal hidden case
%         f(mod(i - 1, n_seeds1) + 3, mod(i - 1, n_seeds2) + n_seeds1 + 2) = 1;
%         f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds1) + 3) = -1;  
%         
%         % intermmediary nodes (equal hidden case)
%         f(mod(i - 1, n_seeds2) + n_seeds1 + 3, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 3) = f(mod(i - 1, n_seeds2) + n_seeds1 + 3, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 3) + 1;
%     else
%         f(mod(i - 1, n_seeds1) + 2, mod(i - 1, n_seeds2) + n_seeds1 + 2) = f(mod(i - 1, n_seeds1) + 2, mod(i - 1, n_seeds2) + n_seeds1 + 2) + 1;
%         f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds1) + 2) = -1;        
%         
%         % intermmediary nodes
%         f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2) = f(mod(i - 1, n_seeds2) + n_seeds1 + 2, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2) + 1;
%         
%         % image 2 to image 3
%         f(mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2, mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + 2) = 1;
%         f(mod(i - 1, n_seeds3) + n_seeds1 + 2 * n_seeds2 + 2, mod(i - 1, n_seeds2) + n_seeds1 + n_seeds2 + 2) = -1;
%     end
    
    
        
    
%     
%     % nodes to sink
    
end

    % nodes to nodes
    
    
    % nodes to sink
%     f(mod(i - 1, n_seeds2) + n_seeds1 + 2, n_seeds1 + n_seeds2 + 2)  = f(mod(i - 1, n_seeds2) + n_seeds1 + 2, n_seeds1 + n_seeds2 + 2) + 1;
    

% % source to nodes
% f(1, 2:1 + n_seeds1) = 1;
% 
% % image 1 to image 2
% f(2:1 + n_seeds1, 2 + n_seeds1:1 + n_seeds1 + n_seeds2) = eye(n_seeds1);
% f(2 + n_seeds1:1 + n_seeds1 + n_seeds2, 2:1 + n_seeds1) = -eye(n_seeds1);
% 
% % intermmediary nodes
% f(2 + n_seeds1:1 + n_seeds1 + n_seeds2, 2 + n_seeds1 + n_seeds2:1 + n_seeds1 + 2 * n_seeds2) = eye(n_seeds2);
% 
% % image 2 to image 3
% f(2 + n_seeds1 + n_seeds2:1 + n_seeds1 + 2 * n_seeds2, 2 + n_seeds1 + 2 * n_seeds2:1 + n_seeds1 + 2 * n_seeds2 + n_seeds2) = eye(n_seeds2);
% f(2 + n_seeds1 + 2 * n_seeds2:1 + n_seeds1 + 2 * n_seeds2 + n_seeds2, 2 + n_seeds1 + n_seeds2:1 + n_seeds1 + 2 * n_seeds2) = -eye(n_seeds2);
% 
% % nodes to sink
% f(n_row - n_seeds3:n_row - 1, n_col) = 1;