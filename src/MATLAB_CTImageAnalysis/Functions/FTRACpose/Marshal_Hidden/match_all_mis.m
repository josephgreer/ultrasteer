function mis=match_all_mis(fpt,ftr,ns_im,n_seeds)
fpt = strcat('Dewarped\Dataset_5\', fpt);
ftr = strcat('Dewarped\Dataset_5\', ftr);
nn=[1,2;...
        2,3;...
        3,1];
N=3;
for i=1:N
%    i
    [c,b]=match_def2(fpt,ftr,[ns_im(nn(i,1)),ns_im(nn(i,2))],n_seeds);
    rand('state',100);
    f=match_rand_flow(n_seeds);
    [f,n_c]=match_cycle(f,b,c);
    fout(i,:)=match_flow2_proc(f,n_seeds);
end
mis=0;
for j=1:n_seeds
    for i=1:N
        if fout(i,j)~=j
            mis=mis+1;
            break;
        end
    end
end
