function g=sm_ellipsegroup4(e,dr_thresh)
%group ellipses using thresholding;

%1.pre-grouping;
[Ne,tmp]=size(e);
Ng=1;
g{Ng,1}=e(1,:);

for i=2:Ne
    tag=0;
    for j=1:Ng
        [Ntmp,tmp]=size(g{j,1});
        tmp=repmat(e(i,:),Ntmp,1)-g{j,1};
        dtmp=sqrt(tmp(:,1).^2+tmp(:,2).^2);
        if min(dtmp)<=dr_thresh
            g{j,1}=[g{j,1};e(i,:)];
            tag=1;
            break;
        end
    end
    if tag==0
        Ng=Ng+1;
        g{Ng,1}=e(i,:);
    end
end

%2.re-grouping;
while (1>0)
	
	Ngn=1;
	gn(Ngn,1)=g(1,1);
	
	for i=2:Ng
        tag=0;
        for j=1:Ngn
            [N1,tmp]=size(gn{j,1});
            [N2,tmp]=size(g{i,1});
            tmp=repmat(gn{j,1}(:,1:2),N2,1)-reshape(repmat(g{i,1}(:,1:2),1,N1)',2,N1*N2)';
            dtmp=sqrt(tmp(:,1).^2+tmp(:,2).^2);
            if min(dtmp)<=dr_thresh
                gn{j,1}=[gn{j,1};g{i,1}];
                tag=1;
                break;
            end
        end
        if tag==0
            Ngn=Ngn+1;
            gn(Ngn,1)=g(i,1);
        end
	end
    
    if Ngn==Ng
        break;
    else
        g=gn;
        Ng=Ngn;
        gn=cell(1);
    end
end
