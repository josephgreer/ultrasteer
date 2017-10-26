clear; clc; close all;


t01 = 90;
t111 = -30;          t112 = 30;
wall11 = [50 300;...
         50 100];
     
wall12 = [-50 300;...
         -50 100];


t02 = 0;
t121 = -30;          t122 = 30;     
wall21 = [300    -50;
         100    -50];
     
wall22 = [300    50;
         100    50];
       
t131 = -30;             t132 = 30;
t03 = 270;
wall31 = [-50       -250;
         -50       -0];
     
wall32 = [50       -250;
         50       -0];
     
t04 = 180;
t141 = -30;             t142 = 30;
wall41 = [-250   50;
         0   50];
     
wall42 = [-250   -50;
         0       -50];
    
wall = wall42;
t_1 = deg2rad(t142);
t_0 = deg2rad(t04);

l_1 = 100;
l_2 = 100;

p0 = [0; 0];
p1 = [cos(t_0); sin(t_0)]*l_1+p0;
p2 = [cos(t_1+t_0); sin(t_1+t_0)]*l_2+p1;

pp = [p0 p1 p2].';

plot(pp(:,1), pp(:,2));
hold all;
scatter(p0(1), p0(2), 'k');
daspect([1 1 1]);

xlim([-300 400]);
ylim([-300 400]);

plot(wall(:,1), wall(:,2), 'r');

pend = wall(1,:).';

la = norm(pend-p0);
lb = l_1;
a = pi-abs(t_1);
b = asin(lb/la*sin(a));
c = pi-a-b;
lc = la*sin(c)/sin(a);


c1 = wrapTo2Pi(atan2(pend(2)-p0(2),pend(1)-p0(1)));
addSign = sign(wrapToPi(t_0-c1));
c = c1+addSign*c;

t_0 = c;
l_2 = lc;

p0 = [0; 0];
p1 = [cos(t_0); sin(t_0)]*l_1+p0;
p2 = [cos(t_1+t_0); sin(t_1+t_0)]*l_2+p1;
pp = [p0 p1 p2].';
plot(pp(:,1), pp(:,2),'b');

grid on;

