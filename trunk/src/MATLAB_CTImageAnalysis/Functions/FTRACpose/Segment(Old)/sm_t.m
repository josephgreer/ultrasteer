%3.find possible ellipses using hough tranform;
ps=sm_getallcontourpoints(Ie);
[Nc,tmp]=size(cts);
ec=[];
for i=1:Nc
    i
    ec=[ec;sm_houghaxislength(cts(i,:),ps)];
end
[Ne,tmp]=size(ec);
ee=zeros(Ne,5);
for k=1:Ne
    if ec(k,7)<=ec(k,8)
        ee(k,:)=[ec(k,2),ec(k,1),ec(k,4),ec(k,7),ec(k,8)];
    else
        ee(k,:)=[ec(k,2),ec(k,1),ec(k,6),ec(k,8),ec(k,7)];
    end
end
figure;
imshow(Ie);
axis on;
hold on;
for k=1:Ne
    sm_drwelips(ee(k,1),ee(k,2),ee(k,3),ee(k,4),ee(k,5));
end
%ge=sm_ellipsegroup(ee,5)