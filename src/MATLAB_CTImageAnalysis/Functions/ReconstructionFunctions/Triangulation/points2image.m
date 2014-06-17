function imagePts = points2image (points, rotation, translation, fc,cc,kc, alpha_c)
% Takes a 3D point and camera parameters and returns the pixel in the image
% that the point projects to. 

%imagePts = zeros(length(points),2);
% Refer to page 58 of spong for this formula
%for i = 1:length(points)
%    imagePts(i,:) = point2image (points(:,i), rotation, translation, fc,cc,kc, alpha_c);
%end


                                              
rotation_vector= rodrigues(rotation);
                                                
imagePts = project_points(points , rotation_vector ,translation,fc,cc,kc);

