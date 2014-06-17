function [stereo.E, stereo.F] = fundamentalMatrixEstimate(BBs85,BBs95)


% Compute F from P's
F = vgg_F_from_P(view(1).P, view(2).P);

% Display
vgg_gui_F(view(1).I, view(2).I, F');
disp('Computed epipolar geometry. Move the mouse to verify')
