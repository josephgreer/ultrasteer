%Function to choose the best possible pose from a list of given transforms.
%The xform with lease err is chosen.
function [T, E, i] = Choose_Transform(transforms, errors)

[E, i] = min(errors);
T = transforms(:,:,i);
        
        