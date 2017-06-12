function tip = kinematicParametersToTipPoint(l,phi,theta,R0,x0)
if(abs(theta) < 1e-7)
    tip = l*R0(:,3)+x0;
    return;
end
r = l/theta;
cen_of_curve = r*R0*[cos(phi); sin(phi); 0]+x0;
delta = x0-cen_of_curve; delta = delta/norm(delta);
tip = r*(cos(theta)*delta+sin(theta)*R0(:,3))+cen_of_curve;
end