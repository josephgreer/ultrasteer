%%
%%%%%%%%%%%%%%%%%%%%%%%% Tube 1
clear; clc; close all;

fname = 'Data/Stiffness/1_9.8.txt';
pressure = sscanf(fname,'Data/Stiffness/%d_%d.%d.txt');
pressure = pressure(2)/10.0+pressure(3)/100.0;      % in psi
pressure = 3*pressure*6894.76;                      % in pascals

numLinks = 1.5;
R0 = 8/2*1e-3;                                    % in m
L0 = 40*1e-3;                            % in m

contractions = linspace(0,0.4,5);
forces = zeros(size(contractions));
for i=1:length(contractions)
    e = contractions(i);
    forces(i) = numLinks*ContractionToForce(e,L0,R0,pressure);
end
plot(contractions,forces);
hold on;

data = load(fname);
fullyContracted = 2*numLinks*L0/pi;
es = zeros(size(data,1),1);
forces = zeros(size(es));
for i=1:size(data,1)
    addedLength = 10-data(i,1); % (in cm)
    addedLength = addedLength*1e-2;
    len = fullyContracted+addedLength;
    es(i) = 1-(len/(numLinks*L0));
    forces(i) = data(i,2)*4.4452016; % in newtons
end
plot(es,forces);

