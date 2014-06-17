function g=sm_linesgroup2(L,dis_thresh,ang_thresh)
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
            dr=abs(L(k,8)-g{i}(:,8));
            dth1=abs(mod(L(k,9)-g{i}(:,9),pi));
            dth2=abs(mod(g{i}(:,9)-L(k,9),pi));
            dth=min([dth1,dth2]');
            if mean(dr)<dis_thresh & mean(dth)<ang_thresh
                p=m+1;
                g{i}(p,:)=[L(k,:),k];
                tag=1;
                break;
            end
        end
        if tag==0
            N_g=N_g+1;
            g{N_g,1}(1,:)=[L(k,:),k];
        end
    end
end
