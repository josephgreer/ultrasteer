%$ID: $
function WriteMappedCoordsToFile(filename, Xmapped, Ymapped)

% function WriteMappedCoordsToFile(filename, Xmapped, Ymapped)
% writes the mapped coordinates to the file. These mapped coordinates are
% calculated using dewarping script. These represent a one-to-one mapping
% of the pixel coordinates from the acquired/warped image to the dewarped
% image. 
% param @filename - name of the file to write the coordinates
% param @Xmapped - Nx1 array with the mapped x coordinates
% param @Ymapped - Nx1 array with the mapped y coordinates
% Serialization format: 
% 1. number of elements (N)
% 2. [Xmapped, Ymapped]
Xmapped = Xmapped(:);
Ymapped = Ymapped(:);

fid = fopen(filename, 'wt');
if (fid < 0) 
    error ('Could not open  the coordinates file');
end

fprintf(fid, '%d\n', size(Xmapped,1));
fprintf(fid, '%f\t%f\n', [Xmapped, Ymapped]');

fclose(fid);