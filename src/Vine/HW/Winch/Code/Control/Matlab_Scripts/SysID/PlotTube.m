function tubeHandles = PlotTube(r1, r2, rmid, theta, tubeHandles)
npoints = 100;
thetas = linspace(0,theta,npoints)';

pointsInner = r1*[cos(thetas) sin(thetas)];
pointsOuter = r2*[cos(thetas) sin(thetas)];

points = [pointsInner; pointsOuter(end:-1:1,:)];
if(~isfield(tubeHandles,'outline'))
    tubeHandles.outline = plot(zeros(2*npoints,1), zeros(2*npoints,1));
end
set(tubeHandles.outline,'XData',points(:,1));
set(tubeHandles.outline,'YData',points(:,2));

points = rmid*[cos(thetas) sin(thetas)];
if(~isfield(tubeHandles,'mid'))
    tubeHandles.midline = plot(zeros(2*npoints,1), zeros(2*npoints,1),'k--');
end
set(tubeHandles.midline,'XData',points(:,1));
set(tubeHandles.midline,'YData',points(:,2));
end