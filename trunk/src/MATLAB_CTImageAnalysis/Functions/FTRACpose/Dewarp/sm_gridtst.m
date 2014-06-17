figure;
imshow(I);
axis on;
hold on;

N=318;
for i=1:N
    plot(p_true(i,2)+p_warp(1,2),p_true(i,1)+p_warp(1,1),'*','color','y');
    plot(p_warp(i,2),p_warp(i,1),'*');
end
