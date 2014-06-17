function ps=sm_pointsofline(p1,p2)

i1=p1(1);
j1=p1(2);
i2=p2(1);
j2=p2(2);

i_min=min(i1,i2);
i_max=max(i1,i2);
j_min=min(j1,j2);
j_max=max(j1,j2);

ii=i_max-i_min;
jj=j_max-j_min;

k=0;
if ii>jj
    %use i as variable;
    i_min=round(i_min);
    i_max=round(i_max);

    for i=i_min:i_max
        j=(j2-j1)/(i2-i1)*(i-i1)+j1;
        k=k+1;
        ps(k,:)=[i,j];
%        plot(ps(k,2),ps(k,1),'*');
    end
else
    %use j as variable;
    j_min=round(j_min);
    j_max=round(j_max);
    
    for j=j_min:j_max
        i=(i2-i1)/(j2-j1)*(j-j1)+i1;
        k=k+1;
        ps(k,:)=[i,j];
%        plot(ps(k,2),ps(k,1),'*');
    end
end
