function p = evalLogistic(x, B)
x = [1 x];
if(size(B,1) == 1)
    B = B';
end
eb = exp(x*B);
p = eb/(1+eb);
end