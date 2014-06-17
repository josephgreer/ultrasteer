function averages

avgs = 0;
for i = 1:20
    [pts_3d,r,res_ra,res_ra_m,res_ra_u,res_bpe,res_bpe_m,res_bpe_u,res_re,res_re_m,res_re_u,res_ren,res_ren_m,res_ren_u]=sm_main_seeds('Dewarped\Dataset_5\', i);
    avgs = avgs + res_re;
end

avgs = avgs / 20;
avgs