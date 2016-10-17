function points = GenerateRandomPointsInWorkspace(npts,kmults, kmain, ktor, leqs, gammas, torGamma, radius, psis, R0, pos0)
ps = 0.3*rand(npts,3);
%[ps1 ps2 ps3] = ndgrid(linspace(0,0.3,npts1),linspace(0,0.3,npts2), linspace(0,0.3,npts3));
% ps1 = ps1(:);
% ps2 = ps2(:);
% ps3 = ps3(:);
% ps = [ps1 ps2 ps3];

points = {};
for i=1:npts
    [tip, RR, theta, l, phi] = PressuresToTip(ps(i,:), kmults, kmain, ktor, leqs, gammas, torGamma, radius, psis, R0, pos0);
    point.pos = tip;
    point.q = RotationMatrixToQuat(RR);
    points = vertcat(points,{point});
end
end