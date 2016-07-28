%%
%%%%%%%%%%%%%%%%%%%%%%%% Tube 1
clear; clc; close all;

files = rdir('Data/Stiffness/1_2.0.txt');

hold all;
for i=1:length(files)
    mode = 'pleated';
    fname = files(i).name;
    display(fname)
    pressure = sscanf(fname,'Data/Stiffness/%d_%d.%d.txt');
    pressure = pressure(2)/10.0+pressure(3)/100.0;      % in psi
    pressure = 3*pressure*6894.76;                      % in pascals
    
    numLinks = 1.5;
    R0 = 8/2*1e-3;                                    % in m
    L0 = 40*1e-3;                                     % in m
    
    E = 0.1*1e9;                                      %elastic modulus pascals
    s = 2e-3*0.0254;                                  % thickness in meters
    A = 2*L0*s;                                       % cross-sectional area in m^2
    a = pi*pressure*R0^2/(A*E);
    
    D0 = numLinks*GetD0(L0,R0,pressure,a,mode);
    
    maxContraction = 1-(D0/(numLinks*L0));
    
    contractions = linspace(0,maxContraction,5);
    forces = zeros(size(contractions));
    for i=1:length(contractions)
        e = contractions(i);
        forces(i) = numLinks*ContractionToForce(e,L0,R0,pressure,a,mode);
    end
    plot(contractions,forces, 'b');
    
    data = load(fname);
    es = zeros(size(data,1),1);
    forces = zeros(size(es));
    forcesReturn = zeros(size(es));
    for i=1:size(data,1)
        addedLength = 10-data(i,1); % (in cm)
        addedLength = addedLength*1e-2;
        len = D0+addedLength;
        es(i) = 1-(len/(numLinks*L0));
        forces(i) = data(i,2)*4.4452016; % in newtons
        forcesReturn(i) = data(i,3)*4.4452016; % in newtons
    end
    plot(es,forces,'r');
    %plot(es,forcesReturn,'r');
end

