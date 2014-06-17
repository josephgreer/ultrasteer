function [total,n_mis]=match_all_mattst(fp,ft,ns_im,n_seeds)

[c,b]=match_def2(fp,ft,ns_im,n_seeds);
%rand('state',100);
f=match_rand_flow(n_seeds);
[f,n_c]=match_cycle(f,b,c);
total=0;
for i=1:2*n_seeds+2
    for j=1:2*n_seeds+2
        if f(i,j)==1
            total=total+c(i,j);
        end
    end
end
fout=match_flow2_proc(f,n_seeds);
n_mis=0;
for i=1:n_seeds
    if fout(i)~=i
        n_mis=n_mis+1;
    end
end
