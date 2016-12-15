function handles = drawVine(xs,d, handles)
n_actuators = 3;
assert(length(xs) == n_actuators || length(xs) == n_actuators+2);

npoints = 100;

thetas = linspace(0,2*pi,npoints).';


hold on;
if(~isfield(handles,'baseCircle'))
    handles.baseCircle = plot3(zeros(npoints,1),zeros(npoints,1),zeros(npoints,1));
end
baseCircle = d*[cos(thetas) sin(thetas) zeros(npoints,1)];
set(handles.baseCircle, 'XData', baseCircle(:,1));
set(handles.baseCircle, 'YData', baseCircle(:,2));
set(handles.baseCircle, 'ZData', baseCircle(:,3));

% main length

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

act_thetas = actuatorThetas();
phis = act_thetas-phi;
rs = r-d*cos(phis);
ts = ls./rs;

center_of_actuators = d*[cos(act_thetas.'); sin(act_thetas.'); zeros(1,length(ls))];
center_of_curves = [cos(phi); sin(phi); 0]*rs.'+center_of_actuators;
deltas = center_of_actuators-center_of_curves;
cen_of_curve = r*[cos(phi); sin(phi); 0];
delta = -cen_of_curve;

if(~isfield(handles,'sideCurves'))
    handles.sideCurves = {};
    for i=1:length(ls)
        handles.sideCurves{i} = plot3(zeros(npoints,1), zeros(npoints,1), zeros(npoints,1));
    end
end
for i=1:length(ls)
    bx = deltas(:,i);
    bx = bx/norm(bx);
    bx = bx.';
    by = [0 0 1];
    tts = linspace(0,ts(i),npoints).';
    pts = rs(i)*(cos(tts)*bx+sin(tts)*by)+repmat(center_of_curves(:,i).',npoints,1);
    set(handles.sideCurves{i},'XData',pts(:,1));
    set(handles.sideCurves{i},'YData',pts(:,2));
    set(handles.sideCurves{i},'ZData',pts(:,3));
end

ncircs = 25;
if(~isfield(handles,'topCircle'))
    handles.topCircle = {};
    for i=1:ncircs
        handles.topCircle{i} = plot3(zeros(npoints,1), zeros(npoints,1), zeros(npoints,1),'Color',[0.5 0.5 0.5 0.5],'LineWidth',2);
        handles.topCircle{i}.Color(4) = 0.5;
    end
end


tts = linspace(0,ll/r,ncircs);

for i=1:ncircs
    bx = delta;
    bx = bx/norm(bx);
    bx = bx.';
    by = [0 0 1];
    theta = tts(i);
    center_of_top_curve = r*(bx*cos(theta)+by*sin(theta))+cen_of_curve.';
    
    basis = Rz(phi)*Ry(theta);
    bx = basis(:,1); by = basis(:,2);
    bx = bx.'; by = by.';
    topCircle = d*(cos(thetas)*bx+sin(thetas)*by)+repmat(center_of_top_curve,npoints,1);
    set(handles.topCircle{i},'XData', topCircle(:,1));
    set(handles.topCircle{i},'YData', topCircle(:,2));
    set(handles.topCircle{i},'ZData', topCircle(:,3));
end

end