function res=sm_matching_3im_pre(c)
%SM_MATCHING_3IM_PRE lists all the matching loops for 3-image matching.
%
%RES=SM_MATCHING_3IM_PRE(C), 
%where C is a cell c is a cell constaining 3 N*N cost matrices for matching.
%sm_matching_3im_pre returns a cell containing all the matching loops.

%implement Hungarian method to all the 2-image combos;
[N,tmp]=size(c);
[n_seeds,tmp]=size(c{1});

for i=1:N
    i
    f=sm_hungarian(c{i})';
    fout(i,:)=f;
end

%a1=0;
%a2=0;
%a3=0;
%a11=0;
%a22=0;
%a33=0;
%for j=1:120
%    a1=a1+c{1}(j,fout(1,j));
%    a2=a2+c{2}(j,fout(2,j));
%    a3=a3+c{3}(j,fout(3,j));
%    a11=a11+c{1}(j,j);
%    a22=a2+c{2}(j,j);
%    a33=a33+c{3}(j,j);
%end


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
