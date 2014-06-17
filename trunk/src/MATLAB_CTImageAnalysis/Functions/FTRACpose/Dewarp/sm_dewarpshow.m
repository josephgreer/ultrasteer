function I_new=sm_dewarpshow(fim,A2,p_warp,p_dewarp,dp_true,n_equ)

figure;
imshow(fim);
axis on;
hold on;
[np,tmp]=size(p_warp);
for i=1:np
    plot(p_warp(i,2),p_warp(i,1),'*');
    plot(dp_true(i,2)+p_warp(1,2),dp_true(i,1)+p_warp(1,1),'*','color','yellow');
end
I_new=sm_imdewarp2(fim,A2,n_equ);
figure;
imshow(I_new);
axis on;
hold on;
for i=1:np
    plot(p_dewarp(i,2),p_dewarp(i,1),'*');
    plot(dp_true(i,2)+p_dewarp(1,2),dp_true(i,1)+p_dewarp(1,1),'*','color','yellow');
end
