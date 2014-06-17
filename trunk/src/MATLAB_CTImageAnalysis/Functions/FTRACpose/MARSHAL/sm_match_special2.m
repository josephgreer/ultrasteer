function mm=sm_match_special2(nnn,c_all,pt,tr,ex,ey,xo,yo,f)
%sm_match_special finds the best matching from a loop longer than 6*6*6.
%
%mm=sm_match_special(nnn,c_all,pt,tr,ex,ey,xo,yo,f),
%where nnn is the input long loop,
%c_all is a cell containing 3 N*N cost matrices,
%pt is a cell containing 3 N*2 matrices of image,
%tr is a cell containing 3 4*4 transform matrics,
%ex and ey are the x,y pixel sizes,
%xO and xO are the x,y image centers,
%f is the focal length.
%sm_match_special returns the best matching list.

[k,N]=size(nnn);
c=cell(3,1);
c{1}=c_all{1}(nnn(1:k,1),nnn(1:k,2));
c{2}=c_all{2}(nnn(1:k,2),nnn(1:k,3));
c{3}=c_all{3}(nnn(1:k,3),nnn(1:k,1));
res1=sm_matching_3im_pre(c);
[n_t,tmp]=size(res1);
res=cell(n_t,1);
nnnn=reshape(nnn',1,N*k);
for i=1:n_t
    tmp=res1{i};
    [m,n]=size(tmp);
    n=n-1;
    m=n/N;
    tmp=reshape(tmp(1:n),N,m)';
    tmp2=zeros(m,N);
    for b=1:N
        tmp2(:,b)=nnn(tmp(:,b),b);
    end
    res{i}=[reshape(tmp2',1,m*N),nnn(res1{i}(n+1),1)];
end

cc=0;
if n_t>1
    for j=1:n_t
        [tmp,n_c]=size(res{j});
        n_c=n_c-1;
        if n_c<=N
            cc=cc+1;
            mm(cc,:)=[res{j}(1:N),0];
        else
            k=n_c/N;
            nn=reshape(res{j}(1:n_c),N,k)';
            if k<7
                bb=sm_comb_all(nn);
                cost=sm_comb_all_cost2(bb,pt,tr,ex,ey,xo,yo,f);
                if k<=5
                    mm(cc+1:cc+k,:)=[sm_combset_match(cost,k,bb),zeros(k,1)];
                else
                    mm(cc+1:cc+k,:)=[sm_combset_match_bypart(cost,k,5,bb),zeros(k,1)];
                end
            else
                mm(cc+1:cc+k,:)=sm_match_special2(nn,c_all,pt,tr,ex,ey,xo,yo,f);
            end
            cc=cc+k;
        end
    end
else
    mmm=nnn;
    while k-cc>=7
        cc=cc+1;
        [mm(cc,:),mmm]=sm_match_assist_2(mmm,pt,tr,ex,ey,xo,yo,f);
    end
%    mm(cc+1:k,:)=sm_match_special(mmm,c_all,pt,tr,ex,ey,xo,yo,f);
    bb=sm_comb_all(mmm);
    cost=sm_comb_all_cost2(bb,pt,tr,ex,ey,xo,yo,f);
    mm(cc+1:k,:)=[sm_combset_match_bypart(cost,k-cc,5,bb),zeros(k-cc,1)];
end
