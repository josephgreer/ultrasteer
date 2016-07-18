%%
%%%%%%%%%%%%%%%%%%%%%%%% Tube 1
clear; clc; close all;

mode = 'pleated';
fname = 'Data/Stiffness/2_6.2.txt';
pressure = sscanf(fname,'Data/Stiffness/%d_%d.%d.txt');
pressure = pressure(2)/10.0+pressure(3)/100.0;      % in psi
pressure = 3*pressure*6894.76;                      % in pascals

numLinks = 2;
R0 = 8/2*1e-3;                                    % in m
L0 = 40*1e-3;                                     % in m
D0 = numLinks*GetD0(L0,R0,pressure,mode);
maxContraction = 1-(D0/(numLinks*L0));

contractions = linspace(0,maxContraction,5);
forces = zeros(size(contractions));
for i=1:length(contractions)
    e = contractions(i);
    forces(i) = numLinks*ContractionToForce(e,L0,R0,pressure,'pleated');
end
plot(contractions,forces);
hold on;

data = load(fname);
es = zeros(size(data,1),1);
forces = zeros(size(es));
for i=1:size(data,1)
    addedLength = 10-data(i,1); % (in cm)
    addedLength = addedLength*1e-2;
    len = D0+addedLength;
    es(i) = 1-(len/(numLinks*L0));
    forces(i) = data(i,2)*4.4452016; % in newtons
end
plot(es,forces);

