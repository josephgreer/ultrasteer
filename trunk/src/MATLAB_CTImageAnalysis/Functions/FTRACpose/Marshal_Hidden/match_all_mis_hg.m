function [n_t,res,n_c]=match_all_mis_hg(fpt,ftr,ns_im,n_seeds)
%list out all the good matches and bad matches for 3 images;

%list out all the 2-image combination;
nn=[1,2;...
        2,3;...
        3,1];

%implement Hungarian method to all the 2-image combos;
N=3;
for i=1:N
    i
    %Hungarian for one combo to generate the flow of one combo;
    f=match_hungarian(fpt,ftr,[ns_im(nn(i,1)),ns_im(nn(i,2))]);
    %make a flow list for all the combos;
    fout(i,:)=f;
end

%find all the matching loops;
id=zeros(N,n_seeds);
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

%count all the mismatches;
mis=0;
for k=1:n_t
    if n_c(1,k)>N
        mis=mis+1;
    elseif res{k,1}(1)~=res{k,1}(2) |res{k,1}(2)~=res{k,1}(3)|res{k,1}(3)~=res{k,1}(1)
        mis=mis+1;
    end
end

good=n_t-mis;