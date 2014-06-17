function res = sm_matching_3im_pre(c)
%SM_MATCHING_3IM_PRE lists all the matching loops for 3-image matching.
%
%RES = SM_MATCHING_3IM_PRE(C), 
%where C is a cell c is a cell constaining 3 N*N cost matrices for matching.
%sm_matching_3im_pre returns a cell containing all the matching loops.

%implement Hungarian method to all the 2-image combos;
N = length(c);
n_seeds = length(c{1});
fout = {};
% n_seeds1 = length(c{1});
% n_seeds2 = length(c{2});
% n_seeds3 = length(c{3});

% add fake seeds
% c{2} = [c{2}, ones(100,1)*999, ones(100,1)*999, ones(100,1)*999];

for i = 1:N
    i
    flipped = 0;
    
    [n_seeds1, n_seeds2] = size(c{i});
    
    % equal number of seeds
    if (n_seeds1 == n_seeds2)
        f = sm_hungarian(c{i})'
        
    % hidden seeds in image 1
    elseif (n_seeds1 < n_seeds2)
        safsaffsaa
        new_c = zeros(n_seeds2, 2*n_seeds1);
        new_c(:,1:2:2*n_seeds1) = c{i}';
        new_c(:,2:2:2*n_seeds1) = c{i}';
        f = sm_hungarian(new_c)';
        f = ceil(f./2);
        f_new = zeros(1,n_seeds1);
        for j = 1:n_seeds2
           f_new(f(j)) = j;
        end
        f = f_new
    % hidden seeds in image 2
    else
        new_c = zeros(n_seeds1, 2*n_seeds2);
        new_c(:,1:2:2*n_seeds2) = c{i};
        new_c(:,2:2:2*n_seeds2) = c{i};
        f = sm_hungarian(new_c)'
        f = ceil(f./2)
    end
    
    fout{i} = f;
    
end

% fout

res = find_matching_loops(fout, N, n_seeds);

% %find all the matching loops;
% id = zeros(N,n_seeds);
% sizeid = size(id)
% n_t = 0;
% 
% for j = 1:n_seeds
%     if id(1,j) == 1
%         continue;
%     end
% 
%     n_t = n_t+1;
% 
%     i = 1;
%     k = j;
% 
%     res{n_t,1}(1,1) = j;
%     cc = 0;
%     while id(i,k) ~= 1
%         cc = cc+1;
%         id(i,k) = 1;
%         k = fout{i}(k);
%         res{n_t,1}(1,cc+1) = k;
%         if i < N
%             i = i+1;
%         else
%             i = 1;
%         end
%     end
%     n_c(1,n_t) = cc;
% end
% 
% % id
% ressize = size(res)
% res{4}