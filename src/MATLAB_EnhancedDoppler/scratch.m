clear; clc; close all;

addpath('C:/ulterius/sdk611/MATLAB/ImagingModes/common');
fname = 'Y:/NeedleScan/PWD_11_21/18-03-15.drf';


nframes = 100000;
[PWRF, header] = RPread(fname, nframes);

% extracting header info
PRF     = header.dr;            % pulse repetition frequency
Fe      = header.txf;           % TX frequency MHz
Fs      = header.sf;            % Sampling frequency MHz
Gate    = header.h;             % Gate size

%PRF=1e6
phi1 = pi/2;
n = 128;
t = [0:1/PRF:(n-1)/PRF];
fs = [1/(1.28e-3) 1/(1.64e-3) 1/(2.64e-3)];
f = fs(1);
a = 5;
B = a*square(f*2*pi*t+phi1);
%A = sin(B).*hamming(n)';
A = (cos(B)+j*sin(B)).*(hamming(n)');
figure;
plot(abs((fft(A))));
figure;
plot(A);
ylim([-1 1]);

f0 = f/(1/max(t))

C = zeros(size(t));
% for i=-5:5
%     C = C+besselj(i,a)*(exp(j*i*2*pi*f*t)-exp(-j*i*2*pi*f*t))/2;%(2*(i-1)+1)*(2*pi*f*t));
% end
% C = C;
for i=-5:5
    C = C+besselj(i,a)*sin(2*pi*i*f*t+phi1);
end
C = C;
figure;
plot(abs(fft(C)));

%C = C.*hamming(n)';
figure;
plot(C);
ylim([-1 1]);
% figure;
% plot(abs(fft(C)));

D = zeros(size(t));
D = sin(a*sin(2*pi*f*t+phi1));
figure;
plot(real(D));
ylim([-1 1]);

% for t=.1:1:100
figure;
bj = [];
js = [-5:1:5];
for j=js
    bj = [bj besselj(j,a)];
end
plot(js,bj);
% end