function [M,cost]=sm_matching_3im(pt,tr,c,pg,ex,ey,xo,yo,f)
%SM_MATCHING_3IM findS all the matching triplets.
%
%SM_MATCHING_3IM(PT,TR,C,PG), where
%PT is a cell of K N-by-2 pixel matrices,
%TR is a cell of K 4-by-4 transform matrices,
%C is a cell of K N-by-N cost matrices,
%PG is a N-by-3 3d truth matrix,
%returns a N-by-4 matching matrix, M, and the cost vector COST.

[N,tmp]=size(pt);
[n_seeds,tmp]=size(pt{1});

res=sm_matching_3im_pre(c);
[n_t,tmp]=size(res);

mm=zeros(n_seeds,N);
cc=0;

for j=1:n_t
    [tmp,n_c]=size(res{j});
    n_c=n_c-1;
    if n_c<=N
        cc=cc+1;
        mm(cc,:)=res{j}(1:N);
    else
        k=n_c/N;
        nn=reshape(res{j}(1:n_c),N,k)';
        if k>=8
            mm(cc+1:cc+k,:)=nn;
        else
            bb=sm_comb_all(nn);
            cost=sm_comb_all_cost(bb,pt,tr,ex,ey,xo,yo,f);
            if k<=5
                mm(cc+1:cc+k,:)=sm_combset_match(cost,k,bb);
            else
                mm(cc+1:cc+k,:)=sm_combset_match_bypart(cost,k,5,bb);
            end
        end
        cc=cc+k;
    end
end

mmm=zeros(n_seeds,N);
mmm(mm(1:n_seeds,1),:)=mm(1:n_seeds,:);

%method1
cost2=sm_comb_all_cost(mmm,pt,tr,ex,ey,xo,yo,f);
%a=[mmm';cost2'];%assisting

%separate good and bad
k1=0;
k2=0;
for i=1:n_seeds
    if cost2(i)<=0.5
        k1=k1+1;
        mm2(k1,:)=[mmm(i,:),0];
    else
        k2=k2+1;
        mm3(k2,:)=mmm(i,:);
    end
end

%process bad
mm4=[];
k5=0;
pt_2d=cell(2,1);

if k2>0
    %process mm3
    k3=0;
    trs{1}=tr{1};
    trs{2}=tr{2};
    for i=1:k2
        pt_2d{1}=pt{1}(mm3(i,1),:);
        pt_2d{2}=pt{2}(mm3(i,2),:);
        [p,d]=sm_recon_core(pt_2d,trs,ex,ey,xo,yo,f)
        p3=sm_backproj_one(p,tr{3},ex,ey,xo,yo,f)
        e2=1000;
        for j=1:k2
            tmp=norm(abs(p3-pt{3}(mm3(j,3),:)));
            if tmp<e2
                e2=tmp;
            end
        end
        if e2<4
            k1=k1+1;
            mm2(k1,:)=[mm3(i,:),-1];
        else
            k3=k3+1;
            mm5(k3,:)=mm3(i,:);
        end
    end
    if k3>0
        %process mm5
        k4=0;
        trs{1}=tr{2};
        trs{2}=tr{3};
        for i=1:k3
            pt_2d{1}=pt{2}(mm5(i,2),:);
            pt_2d{2}=pt{3}(mm5(i,3),:);
            [p,d]=sm_recon_core(pt_2d,trs,ex,ey,xo,yo,f)
            p3=sm_backproj_one(p,tr{1},ex,ey,xo,yo,f)
            e2=1000;
            for j=1:k3
                tmp=norm(abs(p3-pt{1}(mm5(j,1),:)));
                if tmp<e2
                    e2=tmp;
                end
            end
            if e2<4
                k1=k1+1;
                mm2(k1,:)=[mm5(i,:),-2];
            else
                k4=k4+1;
                mm6(k4,:)=mm5(i,:);
            end
        end
        if k4>0
            %process mm6
            k5=0;
            trs{1}=tr{3};
            trs{2}=tr{1};
            for i=1:k4
                pt_2d{1}=pt{3}(mm6(i,3),:);
                pt_2d{2}=pt{1}(mm6(i,1),:);
                [p,d]=sm_recon_core(pt_2d,trs,ex,ey,xo,yo,f)
                p3=sm_backproj_one(p,tr{2},ex,ey,xo,yo,f)
                e2=1000;
                for j=1:k4
                    tmp=norm(abs(p3-pt{2}(mm6(j,2),:)));
                    if tmp<e2
                        e2=tmp;
                    end
                end
                if e2<4
                    k1=k1+1;
                    mm2(k1,:)=[mm6(i,:),-3];
                else
                    k5=k5+1;
                    mm7(k5,:)=mm6(i,:);
                end
            end
            if k5>0
                %process mm7
                if k5>=8
                    mm4=mm7;
                else
%				if k5<=7
                    bb=sm_comb_all(mm7);
                    cost=sm_comb_all_cost(bb,pt,tr,ex,ey,xo,yo,f);
                    if k5<=5
                        mm4=sm_combset_match(cost,k5,bb);
                    else
                        mm4=sm_combset_match_bypart(cost,k5,5,bb);
                    end
                end
            end
        end
	end
end

if k5>0
    mm2=[mm2;[mm4,ones(k5,1)*-4]];
end

cost=zeros(n_seeds,1);
M=zeros(n_seeds,N+1);
M(mm2(1:n_seeds,1),:)=mm2(1:n_seeds,:);
for i=1:n_seeds
    for j=1:N
        pt_2d{j}=pt{j}(M(i,j),:);
    end
    [tmp,cost(i)]=sm_recon_one(pt_2d,tr,M(i,4),ex,ey,xo,yo,f);
end

%mmm=M(:,1:3);
%cost=sm_comb_all_cost(mmm,pt,tr,ex,ey,xo,yo,f);
%b=[mmm';cost2'];%assisting
