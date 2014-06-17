%sm_test

[p,ds]=sm_recon(pt_new,trs,fff(:,4),ex,ey,xo,yo,f);
[a3,b3,c3,d3,e3]=sm_err_recon(p,pts_3d_truth);
sm_h(cost,e3,m_truth);
xlabel('Reconstruction Accurarcy -- Reconstruction Error');
ylabel('Number of Seeds');

p21=sm_backproj(p,trs{1},ex,ey,xo,yo,f);
[a21,b21,c21,d21,e21]=sm_err_backproj(p21,pt_new{1});
sm_h(cost,e21/4,m_truth);
xlabel('Reconstruction Accurarcy -- Backprojection Error 1');
ylabel('Number of Seeds');

p22=sm_backproj(p,trs{2},ex,ey,xo,yo,f);
[a22,b22,c22,d22,e22]=sm_err_backproj(p22,pt_new{2});
sm_h(cost,e22/4,m_truth);
xlabel('Reconstruction Accurarcy -- Backprojection Error 2');
ylabel('Number of Seeds');

p23=sm_backproj(p,trs{3},ex,ey,xo,yo,f);
[a23,b23,c23,d23,e23]=sm_err_backproj(p23,pt_new{3});
sm_h(cost,e23/4,m_truth);
xlabel('Reconstruction Accurarcy -- Backprojection Error 3');
ylabel('Number of Seeds');

sm_h(cost,(e21+e22+e23)/4/3,m_truth);
xlabel('Reconstruction Accurarcy -- Average Backprojection Error');
ylabel('Number of Seeds');

sm_h(e21/4,e3,m_truth);
xlabel('Backprojection Error 1 -- Reconstruction Error');
ylabel('Number of Seeds');

sm_h(e22/4,e3,m_truth);
xlabel('Backprojection Error 2 -- Reconstruction Error');
ylabel('Number of Seeds');

sm_h(e23/4,e3,m_truth);
xlabel('Backprojection Error 3 -- Reconstruction Error');
ylabel('Number of Seeds');

sm_h((e21+e22+e23)/4/3,e3,m_truth);
xlabel('Average Backprojection Error -- Reconstruction Error');
ylabel('Number of Seeds');
