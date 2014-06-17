function res = find_matching_loops(fout, N, n_seeds)

temp = cell(3,1);
temp{1} = fout(:,1);
temp{2} = fout(:,2);
temp{3} = fout(:,3);

n_seeds
fout = temp; 
%find all the matching loops;
id = zeros(N,n_seeds);
sizeid = size(id)
n_t = 0;

for j = 1:n_seeds
    if id(1,j) == 1
        continue;
    end

    n_t = n_t+1;

    i = 1;
    k = j;

    res{n_t,1}(1,1) = j;
    cc = 0;
    while id(i,k) ~= 1
        cc = cc+1;
        id(i,k) = 1;
        k = fout{i}(k);
        res{n_t,1}(1,cc+1) = k;
        if i < N
            i = i+1;
        else
            i = 1;
        end
        i
        k
    end
    n_c(1,n_t) = cc;
end

% id
ressize = size(res)
res{4}


% function res = find_matching_loops(matched, N)
% 
% n_seeds = size(matched, 1);
% id = zeros(N, n_seeds);
% n_t = 0;
% 
% 
% 
% % for j = 1:n_seeds
% %     if id(1,j) == 1
% %         continue;
% %     end
% % 
% %     n_t = n_t + 1;
% % 
% %     i = 1;
% %     k = matched(j,1);
% % 
% %     res{n_t,1}(1,1) = matched(j,1);
% %     
% %     cc = 0;
% %     while id(i,k) ~= 1
% %         cc = cc + 1;
% %         id(i,k) = 1;
% %         k = fout{i}(k);
% %         k = matched(i
% %         res{n_t,1}(1,cc+1) = k;
% %         if i < N
% %             i = i + 1;
% %         else
% %             i = 1;
% %         end
% %     end
% %     n_c(1, n_t) = cc;
% % end
%     
% % %find all the matching loops;
% % id = zeros(N,n_seeds);
% % sizeid = size(id)
% % n_t = 0;
% % 
% % for j = 1:n_seeds
% %     if id(1,j) == 1
% %         continue;
% %     end
% % 
% %     n_t = n_t+1;
% % 
% %     i = 1;
% %     k = j;
% % 
% %     res{n_t,1}(1,1) = j;
% %     cc = 0;
% %     while id(i,k) ~= 1
% %         cc = cc+1;
% %         id(i,k) = 1;
% %         k = fout{i}(k);
% %         res{n_t,1}(1,cc+1) = k;
% %         if i < N
% %             i = i+1;
% %         else
% %             i = 1;
% %         end
% %     end
% %     n_c(1,n_t) = cc;
% % end
% % 
% % % id
% % ressize = size(res)
% % % res{4}
% % 
