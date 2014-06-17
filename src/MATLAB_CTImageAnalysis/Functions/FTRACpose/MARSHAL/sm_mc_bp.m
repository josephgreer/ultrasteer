%call sm_main
t0=clock;
[a1,a2,a3,a4]=sm_main_bp('seed_images_10\seed_images_30\',3)
t1=etime(clock,t0)
save(strcat('seed_images_10\seed_images_30\','res_bp1b.txt'),'t1','a1','a2','a3','a4','-ASCII');
