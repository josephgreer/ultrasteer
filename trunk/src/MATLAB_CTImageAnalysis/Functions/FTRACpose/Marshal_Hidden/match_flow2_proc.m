function fout = match_flow2_proc(f, n_seeds1, n_seeds2)

fout = zeros(1, n_seeds1);
for i = 1:n_seeds1
    ind  = find(f(i+1,:) == 1);
    
    fout(i) = ind - n_seeds1 - 1;
end