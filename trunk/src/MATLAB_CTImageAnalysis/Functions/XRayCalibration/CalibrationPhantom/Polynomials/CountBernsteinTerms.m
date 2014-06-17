function n = CountBarycentricTerms(numVariables, degree)
% function n = CountBernsteinTerms(numVariables, degree)
% return the number of terms in a Bernstein polynomial in numVariables
% variables of degree degree.
n = factorial(numVariables + degree - 1) / prod( [ 1:(numVariables - 1) , 1:degree ] );