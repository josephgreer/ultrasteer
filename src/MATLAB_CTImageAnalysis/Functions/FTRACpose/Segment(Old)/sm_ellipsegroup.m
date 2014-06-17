function ge=sm_ellipsegroup(e,dxy_thresh)
%group ellipses using thresholding;

%dxy_threshold=5;
%dab_threshold=10;
%dth_threshold=pi/180*5;
[Ne,tmp]=size(e);
Nge=1;
ge{1,1}(1,:)=[e(1,:),1];

for k=2:Ne
    tag=0;
    for i=1:Nge
        [m,tmp]=size(ge{i});
        for j=1:m
            dx=abs(e(k,1)-ge{i}(j,1));
            dy=abs(e(k,2)-ge{i}(j,2));
            if sqrt(dx^2+dy^2)<dxy_threshold
                p=m+1;
                ge{i}(p,:)=[e(k,:),k];
                tag=1;
                break;
            end
        end
        if tag==1
            break;
        end
    end
    if tag==0
        Nge=Nge+1;
        ge{Nge,1}(1,:)=[e(k,:),k];
    end
end
