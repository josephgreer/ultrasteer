function [ X_sc ] = scanconvertpoints(X,frm,degPerFr,fpV,probe_R,mX,mY,oX,oY,M,N)
% -------------------------------------------------------------------------
% author: Troy Adebar 
% date: 2012-03-23
%
% title: ScanConvertPoints
%
% summary: This .m file takes an input N x 2 array of XY US image points,
% along with an N x 1 array of slice numbers, and returns a N x 3 array of
% scan converted 3D points. The points are convereted using the supplied
% values for degrees per frame, frames per volume, and image depth in mm.
% -------------------------------------------------------------------------

middle_frm = ceil(fpV/2);

angle = ( frm - middle_frm ).* degPerFr;

X_vec = -(X(:,1)-oX).*mX/1000; % Subtract the origin and scale to mm,...
                                % Note that the image is flipped, so the
                                % negative sign is used to correct

Image_Y = ((X(:,2))-oY).*mY/1000; % Subtract the origin and scale to mm
                                
Y_vec = ( probe_R + Image_Y ).*cosd(angle);
Z_vec = ( probe_R + Image_Y ).*sind(angle);

X_sc = [X_vec Y_vec Z_vec];

end

