%sm_segbat
dir1='Images1\';
dir2='Images3\';
fim1=['IM_00005';...
        'IM_00014';...
        'IM_00022';...
        'IM_00033';...
        'IM_00042';...
        'IM_00074';...
        'IM_00082';...
        'IM_00091';...
        'IM_00100';...
        'IM_00110';...
        'IM_00120'];
x1=[175,353;...
        172,352;...
        177,348;...
        176,350;...
        168,347;...
        175,357;...
        177,347;...
        177,355;...
        179,352;...
        180,349;...
        179,345];
y1=[51,386;...
        62,387;...
        73,386;...
        82,385;...
        100,383;...
        49,390;...
        44,386;...
        42,387;...
        45,387;...
        48,385;...
        50,382];
fim2=['IM_00136';...
        'IM_00143';...%has problem
        'IM_00152';...
        'IM_00160';...
        'IM_00170';...
        'IM_00177';...
        'IM_00185';...
        'IM_00194';...
        'IM_00204';...
        'IM_00214';...
        'IM_00222';...
        'IM_00229';...
        'IM_00237';...
        'IM_00247';...
        'IM_00258';...
        'IM_00266'];
x2=[67,425;...
        68,422;...
        68,425;...
        67,427;...
        69,422;...
        70,424;...
        70,423;...
        69,421;...
        67,425;...
        63,415;...
        62,417;...
        61,419;...
        60,416;...
        59,416;...
        60,421;...
        59,417];
y2=[166,334;...
        170,330;...
        170,330;...
        173,330;...
        171,328;...
        172,328;...
        171,329;...
        174,329;...
        173,327;...
        173,326;...
        174,327;...
        169,330;...
        173,327;...
        173,329;...
        174,328;...
        174,329];
xx11=[11,168;...
        14,169;...
        9,165;...
        10,166;...
        15,175;...
        13,170;...
        11,164;...
        14,162;...
        12,160;...
        10,159;...
        11,157];
yy11=[50,133;...
        44,119;...
        44,103;...
        43,93;...
        40,77;...
        41,140;...
        44,151;...
        44,160;...
        40,166;...
        39,169;...
        40,177];
xx12=[11,163;...
        14,171;...
        10,166;...
        10,165;...
        17,173;...
        12,170;...
        12,160;...
        13,163;...
        11,161;...
        10,158;...
        11,159];
yy12=[133,243;...
        119,237;...
        105,235;...
        94,238;...
        76,233;...
        142,237;...
        155,237;...
        161,233;...
        165,225;...
        170,215;...
        179,212];
xx21=[105,206;...
        104,207;...
        106,206;...
        103,206;...
        102,201;...
        106,203;...
        96,199;...
        101,204;...
        109,210;...
        105,205;...
        105,206;...
        106,206;...
        108,206;...
        110,207;...
        106,207;...
        109,210];
yy21=[13,156;...
        12,153;...
        10,155;...
        9,149;...
        6,150;...
        8,153;...
        8,153;...
        7,149;...
        10,148;...
        8,149;...
        7,147;...
        13,154;...
        7,150;...
        8,148;...
        8,150;...
        6,149];
xx22=[208,312;...
        207,315;...
        204,310;...
        206,311;...
        207,311;...
        203,308;...
        208,312;...
        208,315;...
        210,312;...
        204,308;...
        209,309;...
        208,311;...
        210,313;...
        209,311;...
        208,314;...
        209,311];
yy22=[14,157;...
        11,155;...
        12,154;...
        8,150;...
        9,150;...
        9,153;...
        8,155;...
        6,153;...
        10,150;...
        10,150;...
        9,150;...
        12,156;...
        9,150;...
        10,148;...
        7,149;...
        8,150];

%sm_seg4(dir1,fim1(1,:),x1(1,:),y1(1,:));
sm_seg4('dataset_5\images\','im_00005',[267,568],[108,352]);
[n1,tmp]=size(fim1);
for k=1:n1
    k%=5
    t0=clock;
    sm_autoseg(dir1,fim1(k,:),x1(k,:),y1(k,:),xx11(k,:),yy11(k,:),xx12(k,:),yy12(k,:));
    t=etime(clock,t0)
%    sm_seg4(dir1,fim1(k,:),x1(k,:),y1(k,:));
end
[n2,tmp]=size(fim2);
for k=1:n2
    k%=n2-1;
    t0=clock;
    sm_autoseg(dir2,fim2(k,:),x2(k,:),y2(k,:),xx21(k,:),yy21(k,:),xx22(k,:),yy22(k,:));
    t=etime(clock,t0)
%    sm_seg4(dir2,fim2(k,:),x2(k,:),y2(k,:));
end
