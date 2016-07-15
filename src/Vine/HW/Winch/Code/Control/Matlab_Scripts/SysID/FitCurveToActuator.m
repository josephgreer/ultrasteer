clear; clc; close all;

actuator = 1;
beginningVal = 103;
fname = sprintf('Actuator%d_FlowCalibration.txt', actuator);
data = load(fname);

beginIdx = find(data(:,1) == beginningVal);

data(:,1) = data(:,1)/255;
data(:,2) = 1./data(:,2);
data(:,2) = data(:,2)-min(data(:,2));
data(:,2) = data(:,2)/max(data(:,2));
A = [data(beginIdx:end,1)];
A = [A ones(size(A,1),1)];

mb = A\data(beginIdx:end,2);
m = mb(1); b = mb(2);
data(:,2) = data(:,2)/(m+b);
m = m/(m+b);
b = b/(m+b);
plot(data(:,1), data(:,2));
hold on;

x1 = data(beginIdx,1);
x2 = 1;
y1 = m*x1+b;
y2 = m*x2+b;
plot([x1; x2], [y1; y2], 'r--');

aVal = mean(data(1:beginIdx,2));
x1 = data(1,1);
x2 = data(beginIdx,1);
plot([x1;x2], [aVal; aVal], 'r--');

x_divide = data(beginIdx,1);

npoints = 100;
yvals = [0;1;rand(npoints,1)];
xvals = (yvals-b)/m;
for i=1:length(yvals)
    closestXVal = 1e6;
    closestDist = 1e6;
    
    if(xvals(i) > 1)
        closestDist = abs(m*1+b-yvals(i));
        closestXVal = 1;
    elseif(xvals(i) < x_divide)
        closestDist = abs(m*x_divide+b-yvals(i));
        closestXVal = x_divide;
    else
        closestDist = abs(m*xvals(i)+b-yvals(i));
        closestXVal = xvals(i);
    end
    
    argmn = closestXVal;
    if(abs(aVal-yvals(i)) < closestDist)
        argmn = 0;
    end
    
    display(sprintf('{%f, %f}', yvals(i), argmn));
    scatter(argmn, m*argmn+b);
end
   