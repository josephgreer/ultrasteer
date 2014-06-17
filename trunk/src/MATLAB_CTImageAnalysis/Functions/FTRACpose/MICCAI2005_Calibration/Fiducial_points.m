%Function that defines the fiducial. It needs to be changed if the fiducial
%os changed. Various points that define the fiducial in the fiducial
%coordinate frame are returned. The inputs are the diameter of the fiducial
%and the orientation of the ellipse wrt the central (z)axis of the
%fiducial.
function [model_pts, model_line_pts, model_ellipse1_pts, model_ellipse2_pts] = Fiducial_points(D, th)

len1 = 10 + D*tan(th) + 5 + D*tan(th) + 10;
len2 = 10 + D*tan(th) + 5 + D*tan(th) + 10 - (D/2)*tan(th);

%point features.
model_pts = [-D/2 0 -15;
             -D/2 0 -5;
             -D/2 0 0;
             -D/2 0 len2;
             0 -D/2 -5;
             0 -D/2 0;
             0 -D/2 len1;
             D/2 0 0;
             D/2 0 len2;
             ];
                
%line features.             
model_line_pts = [-D/2 0 0;
                  -D/2 0 len2;
                  0 -D/2 0;
                  0 -D/2 len1;
                  D/2 0 0;
                  D/2 0 len2
                  ];

%ellipse feature in the fom of 12 points each. 5 are reuired to define an ellipse.                  
[model_ellipse1_pts model_ellipse2_pts] = ellipse_points(D, 0.1, th);

%model_pts = [0 -D/2 0;
%             0 -D/2 len1;
%             -D/2 0 0;
%             -D/2 0 len2;
%             D/2 0 0;
%             D/2 0 len2;
%             0 -D/2 -5;
%             -D/2 0 -5;
%             -D/2 0 -15;
%             ];
%model_line_pts = [0 -D/2 0;
%                  0 -D/2 len1;
%                  -D/2 0 0;
%                  -D/2 0 len2;
%                  D/2 0 0;
%                  D/2 0 len2
%                  ];
