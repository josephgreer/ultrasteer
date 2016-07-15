% clear; clc; close all;
% 
% L0 = pi;
% thetas = linspace(1e-6, pi/2, 100);
% 
% h = @(x,y,r)(r*(sin(x)-sin(y)));
% xx = @(x,y,r)(r*cos(x));
% dx = @(x,y,r)(r*sin(x));
% dh = @(x,y,r)(r*cos(x));
% 
% volIntegrand = @(x,y,r)(pi*h(x,y,r).^2.*dx(x,y,r));
% surfaceIntegrand = @(x,y,r)(2*pi*h(x,y,r).*sqrt(dx(x,y,r).^2+dh(x,y,r).^2));
% 
% vols = zeros(size(thetas));
% surfaces = zeros(size(thetas));
% lengths = zeros(size(thetas));
% for i=1:length(thetas)
%     r = L0/(2*thetas(i));
%     y = pi/2-thetas(i);
%     a = y;
%     b = y+2*thetas(i);
%     vols(i) = integral(@(x)volIntegrand(x,y,r),a,b);
%     surfaces(i) = integral(@(x)surfaceIntegrand(x,y,r),a,b);
%     lengths(i) = 2*r*sin(thetas(i));
% end
% figure;
% plot(thetas, vols);
% xlabel('theta');
% ylabel('volume');
% figure;
% plot(thetas, surfaces);
% xlabel('theta');
% ylabel('surface');
% 
% 
% dthetas = diff(thetas);
% dvols = diff(vols)./dthetas;
% dlengths = diff(lengths)./dthetas;
% 
% figure;
% plot(thetas(2:end),-dvols./dlengths,'r');
% xlabel('theta');
% ylabel('force');
% 
% %%
% 
% L0 = pi;
% thetas = linspace(1e-6, pi, 100);
% 
% h = @(x,y,r)(r*(sin(x)-(sin(y))));
% xx = @(x,y,r)(r*cos(x));
% dx = @(x,y,r)(r*sin(x));
% dh = @(x,y,r)(r*cos(x));
% 
% areaIntegrand = @(x,y,r)(h(x,y,r).*dx(x,y,r));
% 
% areas = zeros(size(thetas));
% lengths = zeros(size(thetas));
% for i=1:length(thetas)
%     r = L0/(2*thetas(i));
%     y = pi/2-thetas(i);
%     a = y;
%     b = y+2*thetas(i);
%     areas(i) = integral(@(x)areaIntegrand(x,y,r),a,b);
%     lengths(i) = 2*r*sin(thetas(i));
% end
% figure;
% plot(thetas, areas,'r');
% xlabel('theta');
% ylabel('volume');
% 
% 
% dthetas = diff(thetas);
% dareas = diff(areas)./dthetas;
% dlengths = diff(lengths)./dthetas;
% 
% figure;
% plot(thetas(2:end),dareas./dlengths,'r');
% xlabel('theta');
% ylabel('force');
% 
% %% 
% %%%%%% Pouch motors
% % clear; clc; close all;
% L0 = pi;
% thetas = linspace(1e-2,pi,100);
% D = 1;
% 
% syms t real;
% 
% V = D.*L0^2./(2*t.^2).*(t-sin(t).*cos(t));
% L = L0*sin(t)./t;
% 
% dV = diff(V,t);
% dL = diff(L,t);
% 
% forces = zeros(size(thetas));
% for i=1:length(thetas)
%     dVi = double(subs(dV,t,thetas(i)));
%     dLi = double(subs(dL,t,thetas(i)));
%     forces(i) = dVi/dLi;
% end
% 
% figure;
% plot(thetas,forces);
% figure;
% plot(thetas,double(subs(V,t,thetas)));
% 
% %%
% clear; clc; close all;
% opts = optimset('Diagnostics','off', 'Display','off');
% 
% r0 = 2;
% S0 = 4*pi*r0^2; 
% 
% ecc = @(a,c)(sqrt(1-(a.^2./c.^2)));
% surface = @(a,c)(2*pi*a.^2.*(1+(c./(a.*ecc(a,c))).*asin(ecc(a,c))));
% f = @(a,c)(surface(a,c)-S0);
% minorAxisFromMajorAxis = @(c)(fsolve(@(x)(f(x,c)), 0.5*c,opts));
% strains = @(x)((x-2*r0)/(2*r0));
% 
% lens = linspace(2*r0,10*r0,100);
% vols = zeros(size(lens));
% 
% for i=1:length(lens)
%     c = lens(i)/2;
%     a = minorAxisFromMajorAxis(c);
%     ss = surface(a,c);
%     vols(i) = 4*pi/3*a^2*c;
% end
% 
% plot(strains(lens),vols);
% xlabel('strain');
% ylabel('volume');
% figure;
% plot(strains(lens(2:end)),diff(vols)./diff(lens));
% xlabel('strain');
% ylabel('force');
% 
% figure;
% 
% n = 30;
% hSurf = surf(zeros(n+1),zeros(n+1),zeros(n+1));
% hSurf.LineStyle = 'None';
% daspect([1 1 1]);
% % view(2);
% xlim([-5*r0 5*r0]);
% ylim([-3*r0 3*r0]);
% zlim([-3*r0 3*r0]);
% for i=1:length(lens)
%     c = lens(i)/2;
%     a = minorAxisFromMajorAxis(c);
%     [X,Y,Z] = ellipsoid(0,0,0,c,a,a,n);
%     set(hSurf,'XData',X,'YData',Y,'ZData',Z);
%     pause(1/10);
% end
% 
% %%
% %%%%%%%%% Curve Check
% close all;
% 
% L0 = pi;
% thetas = linspace(1e-3,pi/2,5);
% 
% npoints = 100;
% hPlot = plot(zeros(npoints,1),zeros(npoints,1));
% xlim([-4 4]);
% ylim([-2.1 2.1]);
% daspect([1 1 1]);
% hold on;
% grid on;
% 
% for i=1:length(thetas)
%     r = L0/thetas(i);
%     a = pi/2-thetas(i);
%     ts = linspace(a,a+2*thetas(i),npoints);
%     hs = h(ts,a,r);
%     xs = xx(ts,a,r);
%     ax = gca;
%     plot(xs,hs);
%     ax.ColorOrderIndex = ax.ColorOrderIndex-1;
%     plot(xs,-hs);
% %     set(hPlot,'XData',xs);
% %     set(hPlot,'YData',hs);
% end
% 
% %%
% %%%%%% Constant curvature surfaces
% clear; clc; close all;
% 
% a = 1;
% b = 0.5;
% npoints = 100;
% if(b < a)
%     vs = linspace(-2*pi,2*pi,npoints);
% elseif(b == a)
%     vs = linspace(-pi*a/2,pi*a/2,npoints);
% else
%     vs = linspace(-a*asin(a/b),a*asin(a/b),npoints);
% end
% 
% profileIntegrand = @(x)(sqrt(a.^2-b.^2.*sin(x).^2));
% 
% phis = b.*cos(vs./a);
% psis = zeros(size(phis));
% for i=1:length(phis)
%     psis(i) = integral(profileIntegrand,0,vs(i)/a);
% end
% 
% minx = -max(abs(psis));
% miny = min(phis);
% 
% psis = [minx psis -minx];
% phis = [0 phis 0];
% 
% plot(psis, phis);
% 
% daspect([1 1 1]);
% 
% deltapsi = psis(1:end-1)-psis(2:end);
% deltaphi = phis(1:end-1)-phis(2:end);
% 
% sum(sqrt(deltaphi.^2+deltapsi.^2))

%%
%%%%%%%%% Dearden Method
clear; clc; close all;

circumference = 8e-2;           % m
R0 = 2e-2;                      % m
L0 = 4e-2;                      % m
P = 5*6894.76;                  % Pascals

f = @(x,e)([(2*ellipticE(x(1),x(2))-ellipticF(x(1),x(2)))/(sqrt(x(2))*cos(x(1)))-L0/R0*(1-e);...
            ellipticF(x(1),x(2))/(sqrt(x(2))*cos(x(1)))-L0/R0]);
        

contractions = linspace(0,0.4,20);

Vs = zeros(size(contractions));
phirs = zeros(size(contractions));
ms = zeros(size(contractions));
tractions = zeros(size(contractions));
lengths = zeros(size(contractions));

nPlotPoints = 100;

figure;
hPlot = plot(zeros(2*nPlotPoints,1), zeros(2*nPlotPoints,1));
xlim([-L0 L0]);
ylim([0 L0]);
for i=1:length(contractions)
    display(i);
    e = contractions(i);
    if(e == 0)
        phir = 0;
        m = 0;
    else
        phirm = fsolve(@(x)(f(x,e)),ones(2,1));
        phir = phirm(1);
        m = phirm(2);
    end
    Vs(i) = pi*R0^2*L0/(6*m*cos(phir)^2)*(1-(1-2*m)*(1-e)+2*R0/L0*sqrt(1-(1-2*m*sin(phir)^2)^2));
    phirs(i) = phir;
    ms(i) = m;
    tractions(i) = pi*P*R0^2*(1-2*m)/(2*m*cos(phir)^2);
    
    phis = linspace(0,phir,nPlotPoints);
    xs = R0/(sqrt(m)*cos(phir))*(ellipticE(phis,ones(size(phis)))-1/2*ellipticF(phis,ones(size(phis))));
    rs = R0/cos(phir)*cos(phis);
    
    deltaxs = xs(2:end)-xs(1:end-1);
    deltars = rs(2:end)-rs(1:end-1);
    length = sum(sqrt(deltaxs.^2+deltars.^2))
    lengths(i) = length;
    
    xs = [-flip(xs) xs];
    rs = [flip(rs) rs];
    set(hPlot,'XData',xs);
    set(hPlot,'YData',rs);
    pause(0.1);
end
figure;
hPlot = plot(contractions,Vs);
xlabel('contraction'); ylabel('Volume');
figure;
plot(contractions,phirs);
xlabel('contraction'); ylabel('phir');
figure;
plot(contractions,ms);
xlabel('contraction'); ylabel('m');
figure;
plot(contractions,tractions);
xlabel('contraction'); ylabel('force');
figure;
plot(contractions,lengths);
xlabel('contraction'); ylabel('length');

