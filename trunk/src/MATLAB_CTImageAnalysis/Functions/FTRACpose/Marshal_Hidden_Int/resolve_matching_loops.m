function mm = resolve_matching_loops(pt, cost, tr, ex, ey, xo, yo, f, res, N, n_seeds)

n_t = length(res);


mm = zeros(n_seeds,N+1);
cc = 0;

% combines the results of the 3 different Hungarian algorithms
for j = 1:n_t
    [tmp,n_c] = size(res{j});
    n_c = n_c-1;
    if n_c <= N
        cc = cc+1;
        mm(cc,:) = [res{j}(1:N),0];
    else
        k = n_c/N;
        nn = reshape(res{j}(1:n_c),N,k)';
        if k < 7
            bb = sm_comb_all(nn);
            cost = sm_comb_all_cost2(bb,pt,tr,ex,ey,xo,yo,f);
            if k <= 5
                mm(cc+1:cc+k,:) = [sm_combset_match(cost,k,bb),zeros(k,1)];
            else
                mm(cc+1:cc+k,:) = [sm_combset_match_bypart(cost,k,5,bb),zeros(k,1)];
            end
        else
            mm(cc+1:cc+k,:) = sm_match_special2(nn,cost,pt,tr,ex,ey,xo,yo,f);
        end
        cc = cc+k;
    end
end