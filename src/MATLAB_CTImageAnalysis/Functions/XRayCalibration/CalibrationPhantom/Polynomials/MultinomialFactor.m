function m = MultinomialFactor(p)
% function MultinomialFactor(p)
% Evaluate the multinomial factor \choose{d}{p} = factorial(d) /
% prod(factorial(p)) , where d is the sum of powers in p.
% Repeat for all the rows of p

d = sum(p,2);
if (max(d) > 15)
    error('sum of elements more than 15 -- improper evaluation');
end

pFactorial = factorial2D(p);

numerator = factorial2D(d);
denominator = prod( pFactorial, 2 );

m = numerator ./ denominator;