% To test the various combinations of parameters for the fiducial. Run a
% large loop by calling simulate each time with the chosen value of the selected 
% parameter. Note all the results and analyse.

clear;
num_iter = 1000;

t0 = clock;
mn1 = [];
sd1 = [];
mn2 = [];
sd2 = [];
failures = 0;
for seg_err = 0 : 0.5 : 5
    err1 = [];
    err2 = [];
    for count = 1 : num_iter
        [seg_err count]
        [actual_err1 pose_err1 actual_err2 pose_err2] = Simulate(seg_err);
        if(isempty(actual_err2))
            failures = failures + 1;
        else
            err1 = [err1 ; actual_err1];
            err2 = [err2 ; actual_err2];
        end
    end
    mn1 = [mn1 ; mean(err1)];
    sd1 = [sd1 ; std(err1)];
    mn2 = [mn2 ; mean(err2)];
    sd2 = [sd2 ; std(err2)];
end
t1 = clock;
avg_time = etime(t1, t0)/num_iter;

figure, plot(sqrt(sum(mn1(:,1:3).*mn1(:,1:3),2)))
hold on
plot(sqrt(sum(sd1(:,1:3).*sd1(:,1:3),2)), 'g')

figure, plot(sqrt(sum(mn1(:,4:6).*mn1(:,4:6),2)))
hold on
plot(sqrt(sum(sd1(:,4:6).*sd1(:,4:6),2)), 'g')

figure, plot(sqrt(sum(mn2(:,1:3).*mn2(:,1:3),2)))
hold on
plot(sqrt(sum(sd2(:,1:3).*sd2(:,1:3),2)), 'g')

figure, plot(sqrt(sum(mn2(:,4:6).*mn2(:,4:6),2)))
hold on
plot(sqrt(sum(sd2(:,4:6).*sd2(:,4:6),2)), 'g')
