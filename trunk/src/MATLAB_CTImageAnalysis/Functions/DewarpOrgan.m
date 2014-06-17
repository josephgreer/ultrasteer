function [] = DewarpOrgan(dewarpParams85,dewarpParams95,folderpath,organ)

Folder85 = [folderpath organ 'IMAGES/85/'];
Folder95 = [folderpath organ 'IMAGES/95/'];

mkdir([folderpath organ 'IMAGES/DEWARPED/85']);
mkdir([folderpath organ 'IMAGES/DEWARPED/95']);

imageDewarp(dewarpParams85,Folder85,[folderpath organ 'IMAGES/DEWARPED/85/']);
imageDewarp(dewarpParams95,Folder95,[folderpath organ 'IMAGES/DEWARPED/95/']);
end

% [X map] = gray2ind(cal85j);
% cal85RGB = ind2rgb(X,map);
% 
% [dewarpedImg, dewarpParams] = Dewarp(cal85RGB);


