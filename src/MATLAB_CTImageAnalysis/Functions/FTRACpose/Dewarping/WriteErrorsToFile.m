% $ID $
function WriteErrorsToFile(filename, imName, PolyOrder, mean, min, max, stdev)

fid = fopen(filename, 'At');
fprintf(fid, '%s\n', imName);
fprintf(fid, 'Polynomial degree: %d\n', PolyOrder);
fprintf(fid, '\t Mean: %f\n', mean);
fprintf(fid, '\t Min: %f\n', min);
fprintf(fid, '\t Max: %f\n', max);
fprintf(fid, '\t stdev: %f\n', stdev);

fclose(fid);

% $log $