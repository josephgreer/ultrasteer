
% $Id: generate_all_datasets.m,v 1.1 2006/03/22 00:38:28 jain Exp $

clear
for seed_den = 2.75 : 0.25 : 3
    for pros_size = 40 : 5 : 45
        for cone_angle = 15 : 5 : 20
            for num_iter = 1 : 10
                [seed_den pros_size num_iter cone_angle]
                err = generate_dataset(seed_den, pros_size, 6, cone_angle*pi/180);
                if (sum(sum(err)) > 0.1)
                    display 'Quitting'
                    return
                end
            end
        end
    end
end

% $Log: generate_all_datasets.m,v $
% Revision 1.1  2006/03/22 00:38:28  jain
% Generates all the datasets at once.
%