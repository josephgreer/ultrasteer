function [n_t,mis,res,n_c]=match_all_mis2(fpt,ftr,ns_im,n_seeds)

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


[m,n]=size(fout);
id=zeros(m,n);

n_t=0;
for j=1:n_seeds
    if id(1,j)==1
        continue;
    end

    n_t=n_t+1;

    i=1;
    k=j;

    res{n_t,1}(1,1)=j;
    cc=0;
    while id(i,k)~=1
        cc=cc+1;
        id(i,k)=1;
        k=fout(i,k);
        res{n_t,1}(1,cc+1)=k;
        if i<N
            i=i+1;
        else
            i=1;
        end
    end
    n_c(1,n_t)=cc;
end

mis=0;
for k=1:n_t
    if n_c(1,k)>N
        mis=mis+1;
    elseif res{k,1}(1)~=res{k,1}(2) |res{k,1}(2)~=res{k,1}(3)|res{k,1}(3)~=res{k,1}(1)
        mis=mis+1;
    end
end

good=n_t-mis;