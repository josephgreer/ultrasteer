function oss=sm_recon_general_orientation(pt_2d,ors,trs,ex,ey,xo,yo,f)

[n_ims,tmp]=size(pt_2d);
[n_seeds,tmp]=size(pt_2d{1});

%n_seeds=100;
%n_ims=3;
for i=1:n_ims
    rtmp=trs{i}(1:3,1:3)';%inverse of rotation
    %get the vector between the center point and the perspective center;
    op{i}=pt_2d{i}-repmat([xo(i),yo(i)],n_seeds,1);
    op{i}=op{i}.*repmat([ex(i),ey(i)],n_seeds,1);
    op{i}=[op{i},-f(i)*ones(n_seeds,1)]';
    op{i}=(rtmp*op{i})';
    %get the vector of the planar line seg;
    os{i}=ors{i}.*repmat([ex(i),ey(i)],n_seeds,1);
    os{i}=[os{i},zeros(n_seeds,1)]';
    os{i}=(rtmp*os{i})';
    %get the normal of the plane determined by last two vectors;
    onormal{i}=cross(op{i},os{i},2);
    nn=sqrt(onormal{i}(:,1).^2+onormal{i}(:,2).^2+onormal{i}(:,3).^2);
    onormal{i}=onormal{i}./[nn,nn,nn];
end

oss=zeros(n_seeds,3);
if n_ims==0
    oss=[];
    return;
elseif n_ims==1
    oss=onormal{1};
elseif n_ims==2
    tmp=cross(onormal{1},onormal{2},2);
    nn=sqrt(tmp(:,1).^2+tmp(:,2).^2+tmp(:,3).^2);
    oss=tmp./[nn,nn,nn];
else
    for i=1:n_ims
        if i<n_ims
            tmp=cross(onormal{i},onormal{i+1},2);
            nn=sqrt(tmp(:,1).^2+tmp(:,2).^2+tmp(:,3).^2);
            oss=oss+tmp./[nn,nn,nn];
        else
            tmp=cross(onormal{i},onormal{1},2);
            nn=sqrt(tmp(:,1).^2+tmp(:,2).^2+tmp(:,3).^2);
            oss=oss+tmp./[nn,nn,nn];
        end
    end
	oss=oss/n_ims;
	nn=sqrt(oss(:,1).^2+oss(:,2).^2+oss(:,3).^2);
	oss=oss./[nn,nn,nn];
end
