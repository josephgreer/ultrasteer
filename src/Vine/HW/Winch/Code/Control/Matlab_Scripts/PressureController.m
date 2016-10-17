clear; clc; close all;

fileName = 'SysID/Data/Pressure/Actuator2_1.txt';
fid = fopen(fileName,'r');

us = [];
errors = [];
pAmnts = [];
pressures = [];

tline = fgetl(fid);
while(ischar(tline))
    [A count] = sscanf(tline,'u = %f error %f actuatorAmount %f currPressure %f');
    if(count == 4)
        us = vertcat(us,A(1));
        errors = vertcat(errors,A(2));
        pAmnts = vertcat(pAmnts,A(3));
        pressures = vertcat(pressures,A(4));
    end
    
    tline = fgetl(fid);
end

plot([1:size(pressures,1)],pressures);
figure;
plot([1:size(pressures,1)],pAmnts);
fclose all;