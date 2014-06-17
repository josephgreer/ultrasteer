% To test the various combinations of parameters for the fiducial. Run a
% large loop by calling simulate each time with the chosen value of the selected 
% parameter. Note all the results and analyse.

clear;
num_iter = 500;

t0 = clock;
mn = [];
sd = [];
failures = 0;
for fid_depth = 900 : -50 : 50
    err = [];
    for count = 1 : num_iter
        [fid_depth count]
        [actual_err pose_err] = Simulate(fid_depth);
        if(isempty(actual_err))
            failures = failures + 1;
        else
            err = [err ; actual_err];
        end
    end
    mn = [mn ; mean(err)];
    sd = [sd ; std(err)];
end
t1 = clock;
avg_time = etime(t1, t0)/num_iter;

figure, plot(sqrt(sum(mn(:,1:3).*mn(:,1:3),2)))
hold on
plot(sqrt(sum(sd(:,1:3).*sd(:,1:3),2)), 'g')

figure, plot(sqrt(sum(mn(:,4:6).*mn(:,4:6),2)))
hold on
plot(sqrt(sum(sd(:,4:6).*sd(:,4:6),2)), 'g')
