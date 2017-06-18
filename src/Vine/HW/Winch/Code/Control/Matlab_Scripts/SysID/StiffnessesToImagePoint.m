function [pos,size,handles] = StiffnessesToImagePoint(xs,d,cameraParams, handles)
n_actuators = 3;
assert(length(xs) == n_actuators || length(xs) == n_actuators+2);

if(length(xs) == n_actuators)
    ls = xs;
    ll = mean(ls);
    kappa = lengthsToKappa(ls,d);
    kappa = max(kappa, 1e-6);
    r = 1/kappa;
    phi = lengthsToPhi(ls);
    
    [theta2,ll2,phi2] = lengthsToLThetaPhiNonUniform(ls,d,actuatorThetas());
    r = ll2/theta2;
    phi = phi2;
    kappa = 1/r;
    ll = ll2;
    
    assert(true);%abs(theta2/ll2 - kappa) < 1e-4 && abs(ll2-ll)<1e-4 && min(abs(phi2-phi),2*pi-abs(phi2-phi)) < 1e-4);
end


tip = kinematicParametersToTipPoint(ll,phi,theta2,eye(3),zeros(3,1));

axis = Rz(phi);
axis = axis(:,2)*theta2;
RR = QuatToRotationMatrix(AxisAngleToQuat(axis));

xx = 20*RR(:,1)+tip;
xy = 20*RR(:,2)+tip;
xz = 20*RR(:,3)+tip;

xd = {};
xd{1} = [tip xx]';
xd{2} = [tip xy]';
xd{3} = [tip  xz]';

if(~isfield(handles,'tipFrame'))
    handles.tipFrame = [];
    handles.tipFrame = [handles.tipFrame;plot3(xd{1}(:,1), xd{1}(:,2), xd{1}(:,3),'r', 'LineWidth', 2)];
    handles.tipFrame = [handles.tipFrame;plot3(xd{2}(:,1), xd{2}(:,2), xd{2}(:,3),'g', 'LineWidth', 2)];
    handles.tipFrame = [handles.tipFrame;plot3(xd{3}(:,1), xd{3}(:,2), xd{3}(:,3),'b', 'LineWidth', 2)];
else
    set(handles.tipFrame(1), 'XData', xd{1}(:,1),  'YData',  xd{1}(:,2), 'ZData', xd{1}(:,3));
    set(handles.tipFrame(2), 'XData', xd{2}(:,1),  'YData',  xd{2}(:,2), 'ZData', xd{2}(:,3));
    set(handles.tipFrame(3), 'XData', xd{3}(:,1),  'YData',  xd{3}(:,2), 'ZData', xd{3}(:,3));
end


subplot(1,2,2);
t1 = [cameraParams.targetPosition+[1;0;0]*cameraParams.targetSize;1];
t2 = [cameraParams.targetPosition-[1;0;0]*cameraParams.targetSize;1];

f = cameraParams.focalLength;
C = [f 0 0 0;...
    0 f 0 0;...
    0 0 1 0];
E = [RR tip;...
    zeros(1,3) 1];
E = inv(E);

bad = false;
im1 = C*E*t1;
if(im1(3) < 0)
    bad = true;
end
im1 = im1/im1(3);

im2 = C*E*t2;
if(im2(3)<0)
    bad=true;
end
im2 = im2/im2(3);

size = norm(im2-im1);
if(bad)
    pos = [1e6;1e6];
else
    pos = (im2+im1)/2;
end
end