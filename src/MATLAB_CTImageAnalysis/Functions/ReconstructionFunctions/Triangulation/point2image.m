function imageCoords = point2image (point, rotation, translation, fc,cc,kc, alpha_c)
% Takes a 3D point and camera parameters and returns the pixel in the image
% that the point projects to. 

% Refer to page 58 of spong for this formula
rotation_angle = acos((trace(rotation)-1)/2);

%check to make sure that rotation_angle != 0

if (rotation_angle ~= 0)

    %rotation_vector = rotation_angle *1/(2*sin(rotation_angle)) * [ rotation(3,2) - rotation(2,3);...
                %                                    rotation(1,3) - rotation(3,1);...
                 %                                   rotation(2,1) - rotation(1,2)];
                                                
    rotation_vector= rodrigues(rotation);
                                                
    %rodrigues(rotation_vector);                                         

    
    %imageCoords = project_points2(point , rotation_vector,translation,fc,cc,kc, alpha_c);      
    imageCoords = project_points2(point , rotation_vector ,translation,fc,cc,[kc 0], alpha_c);
else
    imageCoords = [0,0];
end

