addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');

syms t w

R = Rz(w*t)
Rzd = diff(Rz(w*t),t)


ang = simplify(Rzd*R.')
ang2 = simplify(R.'*Rzd)

R = Rx(w*t)
Rxd = diff(Rx(w*t),t)


ang = simplify(Rxd*R.')
ang2 = simplify(R.'*Rxd)

R = Ry(w*t)
Ryd = diff(Ry(w*t),t)


ang = simplify(Ryd*R.')
ang2 = simplify(R.'*Ryd)



