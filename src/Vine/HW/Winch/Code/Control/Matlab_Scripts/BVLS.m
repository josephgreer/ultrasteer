% compute argmin ||Ax-b|| s.t. l <= x <= u
function x = BVLS(A, b, l, u)
m = size(A,1);
n = size(A,2);

done = false;

x = l;
U = find(x >= u);
L = find(x <= l);
F = find(l < x & x < u);

doSteps2Through5 = true;

while(~done)
    if(doSteps2Through5)
        % line 2
        w = A.'*(b-A*x);
        
        % line 3
        posw = find(w > 0);
        negw = find(w < 0);
        
        if(isempty(intersect(negw, U)) && isempty(intersect(posw, L)))
            done = true;
            break;
        end
        
        % line 4
        s = zeros(n,1);
        s(L) = 1; s(U) = -1;
        [~,tstar] = max(s.*w);
        
        % line 5
        F = [F;tstar]; L = setdiff(L,[tstar]); U = setdiff(U,[tstar]);
    end
    
    
    % line 6
    B = union(L,U);
    bprime = b-A(:,B)*x(B);
    Aprime = A(:,F);
    z = Aprime\bprime;
    
    % line 7
    if(all(l(F) <= z & z <= u(F)))
        x(F) = z;
        doSteps2Through5 = true;
        continue;
    end
    
    % line 8
    J = find(z < l(F) | z > u(F));
    if(~isempty(J))
        vals = (z(J) > x(F(J))).*abs((u(F(J))-x(F(J)))./(z(J)-x(F(J)))) + (z(J) <= x(F(J))).*(abs((l(F(J))-x(F(J)))./(z(J)-x(F(J)))));
        
        alpha = min(vals);
%         [~,qp] = min(min(abs((l(F(J))-x(F(J)))./(z(J)-x(F(J)))), abs((u(F(J))-x(F(J)))./(z(J)-x(F(J))))));
%         
%         % line 9
%         alpha = min(abs((l(F(qp))-x(F(qp)))./(z(qp)-x(F(qp)))), abs(u(F(qp))-x(F(qp))./(z(qp)-x(F(qp)))));
        
        % line 10
        x(F) = x(F)+alpha*(z-x(F));
    else
        blah = 0;
    end
    
    % step 11
    U = find(x >= u);
    L = find(x <= l);
    F = find(l < x & x < u);
    x(U) = u(U);
    x(L) = l(L);
    doSteps2Through5 = true;
end

end