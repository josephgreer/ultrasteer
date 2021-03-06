function tip = kinematicParametersToTipPoint(l,phi,theta,R0,x0)
r = l/theta;
cen_of_curve = r*R0*[cos(phi); sin(phi); 0]+x0;
delta = x0-cen_of_curve; delta = delta/norm(delta);
tip = r*(cos(theta)*delta+sin(theta)*R0(:,3))+cen_of_curve;
end