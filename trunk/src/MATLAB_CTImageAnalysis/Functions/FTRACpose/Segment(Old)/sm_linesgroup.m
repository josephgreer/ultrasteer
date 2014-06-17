function g=sm_linesgroup(L,dis_thresh,ang_thresh)
%seperate straight lines into different groups using threshold;

N_g=0;
[N_lines,tmp]=size(L);
for k=1:N_lines
    if N_g==0
        N_g=1;
        g{1,1}(1,:)=[L(k,:),k];
    else
        tag=0;
        for i=1:N_g
            [m,tmp]=size(g{i});
            for j=1:m
                dr=abs(L(k,5)-g{i}(j,5));
                dth1=abs(mod(L(k,6)-g{i}(j,6),pi));
                dth2=abs(mod(g{i}(j,6)-L(k,6),pi));
                dth=min(dth1,dth2);
                if dr<dis_thresh & dth<ang_thresh
                    p=m+1;
                    g{i}(p,:)=[L(k,:),k];
                    tag=1;
                    break;
                end
            end
            if tag==1
                break;
            end
        end
        if tag==0
            N_g=N_g+1;
            g{N_g,1}(1,:)=[L(k,:),k];
        end
    end
end
